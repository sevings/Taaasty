// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Entry.h"

#include <QRegularExpression>
#include <QUrl>
#include <QQmlEngine>

#include "Tlog.h"
#include "Rating.h"
#include "Author.h"
#include "Media.h"
#include "Conversation.h"
#include "Flow.h"
#include "AttachedImage.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../tastydatacache.h"
#include "../models/commentsmodel.h"
#include "../models/messagesmodel.h"
#include "../models/attachedimagesmodel.h"
#include "../models/chatsmodel.h"



EntryBase::EntryBase(QObject* parent)
    : TastyData(parent)
    , _author(nullptr)
    , _type(UnknownEntryType)
{

}



void EntryBase::load(int id)
{
    if (id <= 0)
        return;

    _id = id;
    emit idChanged();

    _request = new ApiRequest(QStringLiteral("v1/entries/%1.json").arg(_id));
    _request->get();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_initBase(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)),  this, SLOT(_maybeError())));

    _initRequest();
}



void EntryBase::_initBase(const QJsonObject& data)
{
    _id     = data.value(QLatin1String("id")).toInt();

    auto authorData = data.value(QLatin1String("author")).toObject();
    if (_author && authorData.contains(QStringLiteral("slug")))
        _author->init(authorData);
    else if (!authorData.isEmpty())
        _author = new Author(authorData, this);
    else
        _author = new Author(this);

    auto type = data.value(QLatin1String("type")).toString();
    if (     type == "image"     || type == "ImageEntry")
        _type = ImageEntry;
    else if (type == "quote"     || type == "QuoteEntry")
        _type = QuoteEntry;
    else if (type == "video"     || type == "VideoEntry")
        _type = VideoEntry;
    else if (type == "text"      || type == "TextEntry" || type == "convo") //! \todo enum for convo?
        _type = TextEntry;
    else if (type == "anonymous" || type == "AnonymousEntry")
        _type = AnonymousEntry;
    else
        _type = UnknownEntryType;

    Q_ASSERT(_type != UnknownEntryType);

    _text   = data.value(QLatin1String("text")).toString().trimmed();
    _title  = data.value(QLatin1String("title")).toString().trimmed();

    emit idChanged();
    emit loaded();
}



void EntryBase::_maybeError()
{
    if (!_author)
        emit loadingError();
}



EntryBase::EntryType EntryBase::type() const
{
    return _type;
}



QString EntryBase::strType() const
{
    switch (_type)
    {
    case ImageEntry:
        return "image";
    case QuoteEntry:
        return "quote";
    case VideoEntry:
        return "video";
    case TextEntry:
        return "text";
    case AnonymousEntry:
        return "anonymous";
    default:
        return QString();
    }
}



QString EntryBase::title() const
{
    return _title;
}



QString EntryBase::text() const
{
    return _text;
}



Author* EntryBase::author() const
{
    return _author;
}



Entry::Entry()
    : EntryBase()
    , _isVotable(false)
    , _isWatchable(false)
    , _isWatched(false)
    , _isFavoritable(false)
    , _isFavorited(false)
    , _isPrivate(false)
    , _isFixed(false)
    , _isReportable(false)
    , _tlog(new Tlog(this))
    , _rating(new Rating(this))
    , _commentsCount(0)
    , _media(new Media(this))
    , _preview(nullptr)
    , _wordCount(0)
    , _commentsModel(nullptr)
    , _attachedImagesModel(new AttachedImagesModel(this))
{
    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Entry::Entry(Conversation* chat)
    : EntryBase()
    , _isVotable(false)
    , _isWatchable(false)
    , _isWatched(false)
    , _isFavoritable(false)
    , _isFavorited(false)
    , _isPrivate(false)
    , _isFixed(false)
    , _isReportable(false)
    , _tlog(nullptr)
    , _rating(new Rating(this))
    , _commentsCount(0)
    , _media(nullptr)
    , _preview(nullptr)
    , _wordCount(0)
    , _commentsModel(nullptr)
    , _attachedImagesModel(nullptr)
{
    if (chat)
        _chatId = chat->id();

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Entry::~Entry()
{
    pTasty->dataCache()->removeEntry(_id);
}



CommentsModel* Entry::commentsModel()
{
    if (_commentsModel && _commentsModel->entryId() == _id)
        return _commentsModel;

    delete _commentsModel;
    _commentsModel = new CommentsModel(this);
    _commentsModel->init(_commentsData, _commentsCount);
    _commentsData = QJsonArray();

    Q_TEST(connect(_commentsModel,    SIGNAL(totalCountChanged(int)),   this, SLOT(_setCommentsCount(int))));

    return _commentsModel;
}



void Entry::init(const QJsonObject& data, bool force)
{
    _initBase(data);

    _createdAt       = Tasty::parseDate(data.value(QLatin1String("created_at")).toString());
    _isVotable       = data.value(QLatin1String("is_voteable")).toBool();
    _isFavoritable   = data.value(QLatin1String("can_favorite")).toBool(pTasty->isAuthorized());
    _isFavorited     = data.value(QLatin1String("is_favorited")).toBool();
    _isWatchable     = data.value(QLatin1String("can_watch")).toBool();
    _isWatched       = data.value(QLatin1String("is_watching")).toBool();
    _isPrivate       = data.value(QLatin1String("is_private")).toBool();
    _isFixed         = data.value(QLatin1String("fixed_state")).toString() == "fixed";
    _isReportable    = data.value(QLatin1String("can_report")).toBool(pTasty->isAuthorized());

    auto fixDate = data.value(QLatin1String("fixed_up_at")).toString();
    _fixedAt         = QDateTime::fromString(fixDate.left(19), "yyyy-MM-ddTHH:mm:ss");

    auto tlogData = data.value(QLatin1String("tlog")).toObject();
    if (!_tlog)
        _tlog        = new Tlog(tlogData, this);
    else if (_tlog->slug().isEmpty() || tlogData.contains(QStringLiteral("slug")))
        _tlog->init(tlogData);

    _url             = data.value(QLatin1String("entry_url")).toString();
    if (_url.isEmpty())
    {
        if (_type == AnonymousEntry)
            _url = QStringLiteral("http://taaasty.com/anonymous/%2").arg(_id);
        else if (!_tlog->tag().isEmpty())
            _url = QStringLiteral("http://taaasty.com/%1/%2").arg(_tlog->tag()).arg(_id);
    }

    auto isMy = _author->id() == pTasty->settings()->userId();
    auto isModer = _tlog->flow() && _tlog->flow()->isEditable();
    _isDeletable     = data.value(QLatin1String("can_delete")).toBool(isMy || isModer);
    _isEditable      = data.value(QLatin1String("can_edit")).toBool(isMy);

    if (!_rating->id())
        _rating->init(data.value(QLatin1String("rating")).toObject());

    _commentsCount   = data.value(QLatin1String("comments_count")).toInt();

    if (data.contains(QStringLiteral("title_truncated")))
        _truncatedTitle = data.value(QLatin1String("title_truncated")).toString();
    else if (_truncatedTitle.isEmpty() || force)
        _truncatedTitle = Tasty::truncateHtml(_title, 100);

    if (data.contains(QStringLiteral("text_truncated")))
        _truncatedText  = data.value(QLatin1String("text_truncated")).toString();
    else if (_truncatedText.isEmpty() || force)
        _truncatedText = Tasty::truncateHtml(_text, 300);

    _source          = data.value(QLatin1String("source")).toString(); // quote author

    delete _media;
    if (_type == VideoEntry)
        _media = new Media(data.value(QLatin1String("iframely")).toObject(), this);
    else
        _media = nullptr;

    delete _preview;
    if (data.contains(QLatin1String("preview_image")))
        _preview = new AttachedImage(data.value(QLatin1String("preview_image")).toObject(), this);
    else
        _preview = nullptr;

    _wordCount   = _countWords(_title + ' ' + _text);

    _correctHtml();

    pTasty->dataCache()->addEntry(sharedFromThis());

    _commentsData    = data.value(QLatin1String("comments")).toArray();
    if (_commentsModel && _commentsModel->entryId() == _id)
    {
        _commentsModel->init(_commentsData, _commentsCount);
        _commentsData = QJsonArray();
    }

    auto imageAttach = data.value(QLatin1String("image_attachments")).toArray();
    delete _attachedImagesModel;
    _attachedImagesModel = new AttachedImagesModel(imageAttach, this);

    _rating->reCalcBayes();

    emit updated();
    emit commentsCountChanged();
}



void Entry::setId(const int id)
{
    if (id <= 0)
        return;

    _id = id;
    emit idChanged();

    _chatId = 0;
    _chat.clear();

    reload();

    Q_TEST(connect(_request, SIGNAL(destroyed()), this, SLOT(_maybeError())));
}



void Entry::reload()
{
    if (isLoading())
        return;

    auto url = QStringLiteral("v1/entries/%1.json?include_comments=true").arg(_id);
    _request = new ApiRequest(url);
    _request->get();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Entry::addComment(const QString& text)
{
    if (_entryRequest || _id <= 0 || !Tasty::instance()->isAuthorized())
        return;

    _entryRequest = new ApiRequest("v1/comments.json", ApiRequest::AllOptions);
    _entryRequest->addFormData("entry_id", _id);
    _entryRequest->addFormData("text", text.trimmed());
    _entryRequest->post();

    Q_TEST(connect(_entryRequest, SIGNAL(success(const QJsonObject)),                this,   SIGNAL(commentAdded(const QJsonObject))));
    Q_TEST(connect(_entryRequest, SIGNAL(networkError(QNetworkReply::NetworkError)), this,   SIGNAL(addingCommentError())));
    Q_TEST(connect(_entryRequest, SIGNAL(error(const int, const QString)),           this,   SIGNAL(addingCommentError())));
    Q_TEST(connect(_entryRequest, SIGNAL(success(const QJsonObject)),                this,   SLOT(_setWatched())));

    Q_TEST(connect(_entryRequest, &QObject::destroyed,
                   this, &Entry::sendingChanged, Qt::QueuedConnection));

    emit sendingChanged();

    ChatsModel::instance()->addChat(sharedFromThis());
}



void Entry::watch()
{
    if (_entryRequest || _id <= 0)
        return;

    if (_isWatched)
    {
        auto url      = QStringLiteral("v1/watching.json?entry_id=%1").arg(_id);
        _entryRequest = new ApiRequest(url, ApiRequest::AllOptions);
        _entryRequest->deleteResource();
    }
    else
    {
        auto url      = QStringLiteral("v1/watching.json");
        _entryRequest = new ApiRequest(url, ApiRequest::AllOptions);
        _entryRequest->addFormData("entry_id", _id);
        _entryRequest->post();
    }

    Q_TEST(connect(_entryRequest, SIGNAL(success(const QJsonObject)), this, SLOT(_changeWatched(QJsonObject))));

    Q_TEST(connect(_entryRequest, &QObject::destroyed,
                   this, &Entry::sendingChanged, Qt::QueuedConnection));

    emit sendingChanged();
}



void Entry::favorite()
{
    if (_entryRequest || _id <= 0)
        return;

    if (_isFavorited)
    {
        auto url      = QStringLiteral("v1/favorites.json?entry_id=%1").arg(_id);
        _entryRequest = new ApiRequest(url, ApiRequest::AllOptions);
        _entryRequest->deleteResource();
    }
    else
    {
        auto url      = QStringLiteral("v1/favorites.json");
        _entryRequest = new ApiRequest(url, ApiRequest::AllOptions);
        _entryRequest->addFormData("entry_id", _id);
        _entryRequest->post();
    }

    Q_TEST(connect(_entryRequest, SIGNAL(success(const QJsonObject)), this, SLOT(_changeFavorited(QJsonObject))));

    Q_TEST(connect(_entryRequest, &QObject::destroyed,
                   this, &Entry::sendingChanged, Qt::QueuedConnection));

    emit sendingChanged();
}



void Entry::report()
{
    if (_entryRequest || _id <= 0)
        return;

    auto url      = QStringLiteral("v1/entries/%1/report.json").arg(_id);
    _entryRequest = new ApiRequest(url, ApiRequest::AllOptions);
    _entryRequest->post();

    Q_TEST(connect(_entryRequest, &QObject::destroyed,
                   this, &Entry::sendingChanged, Qt::QueuedConnection));

    emit sendingChanged();

    auto ptr = sharedFromThis();
    Q_TEST(connect(_entryRequest, static_cast<void(ApiRequest::*)(const QJsonObject&)>(&ApiRequest::success),
                   [ptr](const QJsonObject& data)
    {
        if (!ptr)
            return;

        ptr->_isReportable = data.value(QLatin1String("can_report")).toBool();
        emit ptr->isReportableChanged();

        emit pTasty->info(QStringLiteral("Жалоба на пост отправлена"));
    }));
}



void Entry::deleteEntry()
{
    if (isLoading() || _id <= 0 || !_isDeletable)
        return;

    auto url = QStringLiteral("v1/entries/%1.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->deleteResource();

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_deleteEntry(QJsonObject))));

    _initRequest();
}



void Entry::_changeWatched(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")).toString() != "success")
    {
        qDebug() << data;
        return;
    }

    _isWatched = !_isWatched;
    emit watchedChanged();
}



void Entry::_changeFavorited(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")).toString() != "success")
    {
        qDebug() << data;
        return;
    }

    _isFavorited = !_isFavorited;
    emit favoritedChanged();

    if (_isFavorited)
        emit Tasty::instance()->info(QStringLiteral("Запись добавлена в избранное"));
    else
        emit Tasty::instance()->info(QStringLiteral("Запись удалена из избранного"));
}



void Entry::_setCommentsCount(int tc)
{
    _commentsCount = tc;
    emit commentsCountChanged();
}



void Entry::_setWatched()
{
    if (_isWatched || !_isWatchable)
        return;

    _isWatched = true;
    emit watchedChanged();
}



void Entry::_correctHtml()
{
    Tasty::correctHtml(_title);
    Tasty::correctHtml(_text);

    emit htmlUpdated();
}



void Entry::_setChatId()
{
    auto chat = dynamic_cast<Conversation*>(sender());
    if (chat)
        _chatId = chat->id();
}



void Entry::_deleteEntry(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")) == "success")
    {
        emit entryDeleted();
        emit Tasty::instance()->entryDeleted(_id);
        emit Tasty::instance()->info(QStringLiteral("Запись удалена"));

        _chat.clear();
    }
    else
        qDebug() << data;
}



int Entry::_countWords(QString content)
{
    static QRegularExpression wsRe(QStringLiteral("[^a-zA-Zа-яА-ЯёЁ\\d&#;]+"));
    static QRegularExpression linkRe(QStringLiteral("(?:http[s]?:\\/\\/[^\\s]*)"));
    static QRegularExpression tagRe(QStringLiteral("<[^>]*>"));

    return content.remove(tagRe).remove(linkRe).count(wsRe);
}



bool Entry::isVotable() const
{
    return _isVotable;
}



bool Entry::isPrivate() const
{
    return _isPrivate;
}



bool Entry::isFixed() const
{
    return _isFixed;
}



QDateTime Entry::fixedAt() const
{
    return _fixedAt;
}



int Entry::chatId() const
{
    return _chatId;
}



bool Entry::isSending() const
{
    return _entryRequest;
}



int Entry::commentsCount() const
{
    return _commentsCount;
}



void Entry::resetChat()
{
    _chat.clear();
}



Conversation* Entry::chat()
{
    if (_chat)
        return _chat.data();

    if (_id <= 0 || !pTasty->isAuthorized())
        return nullptr;

    _chat = pTasty->dataCache()->chatByEntry(_id);
    if (_chat)
        return _chat.data();

    _chat = ChatPtr::create(nullptr);
    _chat->setEntryId(_id);

    Q_TEST(connect(_chat.data(), SIGNAL(updated()), this, SLOT(_setChatId())));

    return _chat.data();
}



Tlog* Entry::tlog() const
{
    return _tlog;
}



AttachedImage*Entry::preview() const
{
    return _preview;
}



Rating* Entry::rating() const
{
    return _rating;
}



int Entry::wordCount() const
{
    return _wordCount;
}
