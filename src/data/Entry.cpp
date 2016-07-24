#include "Entry.h"

#include <QRegularExpression>
#include <QUrl>

#include "../defines.h"

#include "Tlog.h"
#include "Rating.h"
#include "Author.h"
#include "Media.h"
#include "Conversation.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../models/commentsmodel.h"
#include "../models/messagesmodel.h"
#include "../models/attachedimagesmodel.h"



EntryBase::EntryBase(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _author(nullptr)
{

}



void EntryBase::load(int id)
{
    if (id <= 0)
        return;

    _id = id;

    auto request = new ApiRequest(QString("v1/entries/%1.json").arg(_id));
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_initBase(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_maybeError())));
}



void EntryBase::_initBase(QJsonObject data)
{
    _id     = data.value("id").toInt();
    _author = new Author(data.value("author").toObject(), this);
    _type   = data.value("type").toString();
    _text   = data.value("text").toString().trimmed();
    _title  = data.value("title").toString().trimmed();

    emit loaded();
}



void EntryBase::_maybeError()
{
    if (!_author)
        emit loadingError();
}



QString EntryBase::type() const
{
    return _type;
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



Entry::Entry(QObject* parent)
    : EntryBase(parent)
    , _isVotable(false)
    , _isWatchable(false)
    , _isWatched(false)
    , _isFavoritable(false)
    , _isFavorited(false)
    , _isPrivate(false)
    , _tlog(new Tlog(this))
    , _rating(new Rating(this))
    , _commentsCount(0)
    , _media(new Media(this))
    , _wordCount(0)
//    , _commentsModel(new CommentsModel(this))
    , _chat(new Conversation(this))
    , _attachedImagesModel(new AttachedImagesModel(this))
    , _loading(false)
{

}



Entry::Entry(const QJsonObject data, Conversation* chat)
    : EntryBase(chat)
//    , _commentsModel(nullptr)
    , _chat(chat)
    , _attachedImagesModel(nullptr)
    , _loading(false)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Entry::Entry(const QJsonObject data, QObject *parent)
    : EntryBase(parent)
//    , _commentsModel(nullptr)
    , _chat(nullptr)
    , _attachedImagesModel(nullptr)
    , _loading(false)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



void Entry::setId(const int id)
{
    if (id <= 0)
        return;

    _id = id;

    reload();

    if (_chat)
        _chat->setEntryId(_id);
}



void Entry::reload()
{
    auto request = new ApiRequest(QString("v1/entries/%1.json").arg(_id));
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));

    _loading = true;
    emit loadingChanged();
}



void Entry::addComment(const QString text)
{
    if (_loading || _id <= 0)
        return;

    _loading = true;
//    emit loadingChanged();
    
    auto content = QUrl::toPercentEncoding(text.trimmed());
    auto data    = QString("entry_id=%1&text=%2").arg(_id).arg(QString::fromUtf8(content));
    auto request = new ApiRequest("v1/comments.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(commentAdded(const QJsonObject))));
    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_setWatched())));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),        this, SLOT(_setNotLoading())));
}



void Entry::watch()
{
    if (_loading || _id <= 0)
        return;

    _loading = true;
//    emit loadingChanged();
    
    ApiRequest* request = nullptr;
    if (_isWatched)
    {
        auto url = QString("v1/watching.json?entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);
    }
    else
    {
        auto url = QString("v1/watching.json");
        auto data = QString("entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);
    }

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeWatched(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),        this, SLOT(_setNotLoading())));
}



void Entry::favorite()
{
    if (_loading || _id <= 0)
        return;

    _loading = true;
//    emit loadingChanged();
    
    ApiRequest* request = nullptr;
    if (_isFavorited)
    {
        auto url = QString("v1/favorites.json?entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);
    }
    else
    {
        auto url = QString("v1/favorites.json");
        auto data = QString("entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);
    }

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeFavorited(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),        this, SLOT(_setNotLoading())));
}



void Entry::_init(const QJsonObject data)
{
    _initBase(data);

    _createdAt       = Tasty::parseDate(data.value("created_at").toString());
    _url             = data.value("entry_url").toString();
    _isVotable       = data.value("is_voteable").toBool();
    _isFavoritable   = data.value("can_favorite").toBool();
    _isFavorited     = data.value("is_favorited").toBool();
    _isWatchable     = data.value("can_watch").toBool();
    _isWatched       = data.value("is_watching").toBool();
    _isPrivate       = data.value("is_private").toBool();
    _tlog            = new Tlog(data.value("tlog").toObject(), this);
    _rating          = new Rating(data.value("rating").toObject(), this);
    _commentsCount   = data.value("comments_count").toInt();
    _truncatedTitle  = data.value("title_truncated").toString();
    _truncatedText   = data.value("text_truncated").toString();
    _source          = data.value("source").toString();
    _media           =  _type == "video" ? new Media(data.value("iframely").toObject(), this)
                                         : nullptr; // music?
//    _imagePreview    = data.value("preview_image").toObject();

    _correctHtml();

    QRegularExpression wordRe("\\s[^\\s]+\\s");
    QRegularExpression tagRe("<[^>]*>");
//    QRegularExpression wordRe("\\b\\w+\\b");
    auto content = _title + _text;
    _wordCount   = content.remove(tagRe).count(wordRe);

//    delete _commentsModel;
//    _commentsModel = new CommentsModel(this);

//    Q_TEST(connect(_commentsModel, SIGNAL(totalCountChanged(int)), this, SLOT(_setCommentsCount(int))));

//    delete _chat;
    if (!_chat)
        _chat = new Conversation(this);

    Q_TEST(connect(_chat->messages(), SIGNAL(totalCountChanged(int)), this, SLOT(_setCommentsCount(int))));

    auto imageAttach = data.value("image_attachments").toArray();
    delete _attachedImagesModel;
    _attachedImagesModel = new AttachedImagesModel(&imageAttach, this);

    _rating->reCalcBayes();

    emit updated();
    emit commentsCountChanged();

    _loading = false;
    emit loadingChanged();
}



void Entry::_changeWatched(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
    {
        qDebug() << data;
        return;
    }

    _isWatched = !_isWatched;
    emit watchedChanged();
}



void Entry::_changeFavorited(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
    {
        qDebug() << data;
        return;
    }

    _isFavorited = !_isFavorited;
    emit favoritedChanged();
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



void Entry::_setNotLoading()
{
    _loading = false;
    emit loadingChanged();

    if (!_tlog || _tlog->tlogId() <= 0)
        emit updatingError(); // TODO: emit it only after setId()
}

int Entry::commentsCount() const
{
    return _commentsCount;
}



Tlog* Entry::tlog() const
{
    return _tlog;
}



bool Entry::loading() const
{
    return _loading;
}



Rating* Entry::rating() const
{
    return _rating;
}



int Entry::wordCount() const
{
    return _wordCount;
}
