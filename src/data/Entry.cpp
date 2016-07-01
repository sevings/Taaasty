#include "Entry.h"

#include <QRegularExpression>
#include <QUrl>

#include "../defines.h"

#include "Tlog.h"
#include "Rating.h"
#include "Author.h"
#include "Media.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../models/commentsmodel.h"
#include "../models/attachedimagesmodel.h"



Entry::Entry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _commentsModel(nullptr)
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

    auto request = new ApiRequest(QString("entries/%1.json").arg(_id));
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
    auto request = new ApiRequest("comments.json", true,
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
        auto url = QString("watching.json?entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);
    }
    else
    {
        auto url = QString("watching.json");
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
        auto url = QString("favorites.json?entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);
    }
    else
    {
        auto url = QString("favorites.json");
        auto data = QString("entry_id=%1").arg(_id);
        request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);
    }

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeFavorited(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),        this, SLOT(_setNotLoading())));
}



void Entry::_init(const QJsonObject data)
{
    _id              = data.value("id").toInt();
    _createdAt       = Tasty::parseDate(data.value("created_at").toString());
    _url             = data.value("entry_url").toString();
    _type            = data.value("type").toString();
    _isVotable       = data.value("is_voteable").toBool();
    _isFavoritable   = data.value("can_favorite").toBool();
    _isFavorited     = data.value("is_favorited").toBool();
    _isWatchable     = data.value("can_watch").toBool();
    _isWatched       = data.value("is_watching").toBool();
    _isPrivate       = data.value("is_private").toBool();
    _tlog            = new Tlog(data.value("tlog").toObject(), this);
    _author          = new Author(data.value("author").toObject(), this);
    _rating          = new Rating(data.value("rating").toObject(), this);
    _commentsCount   = data.value("comments_count").toInt();
    _title           = data.value("title").toString().trimmed();
    _truncatedTitle  = data.value("title_truncated").toString();
    _text            = data.value("text").toString().trimmed();
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

    delete _commentsModel;
    _commentsModel = new CommentsModel(this);

    Q_TEST(connect(_commentsModel, SIGNAL(totalCountChanged(int)), this, SLOT(_setCommentsCount(int))));

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
