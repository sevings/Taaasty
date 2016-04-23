#include "datastructures.h"

#include "apirequest.h"
#include "commentsmodel.h"
//#include "tasty.h"



Entry::Entry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _commentsModel(nullptr)
{
    _id              = data.value("id").toInt();
    _createdAt       = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    _url             = data.value("url").toString();
    _type            = data.value("type").toString();
    _isVotable       = data.value("is_voteable").toBool();
    _isFavoritable   = data.value("can_favorite").toBool();
    _isFavorited     = data.value("is_favorited").toBool();
    _isWatchable     = data.value("can_watch").toBool();
    _isWatched       = data.value("is_watching").toBool();
    _isPrivate       = data.value("is_private").toBool();
    _tlog            = data.value("tlog").toObject();
    _author          = data.value("author").toObject();
    _rating          = new Rating(data.value("rating").toObject(), this);
    _commentsCount   = data.value("comments_count").toInt();
    _title           = data.value("title").toString();
    _truncatedTitle  = data.value("title_truncated").toString();
    _text            = data.value("text").toString();
    _truncatedText   = data.value("text_truncated").toString();
    _imageAttach     = data.value("image_attachments").toArray();
    _imagePreview    = data.value("preview_image").toObject();
}

CommentsModel *Entry::commentsModel()
{
    if (!_commentsModel)
        _commentsModel = new CommentsModel(this);

    return _commentsModel;
}



void Entry::addComment(const QString text)
{
    auto data = QString("entry_id=%1&text=%2").arg(_id).arg(text);
    auto request = new ApiRequest("comments.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(commentAdded(const QJsonObject)));
    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addComment()));
}



void Entry::watch()
{
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

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeWatched(QJsonObject)));
}



void Entry::favorite()
{
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

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeFavorited(QJsonObject)));
}



void Entry::_addComment()
{
    _commentsCount++;
    emit commentsCountChanged();
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



Comment::Comment(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);
}



void Comment::edit(const QString text)
{
    auto url = QString("comments/%1.json").arg(_id);
    auto data = QString("text=%1").arg(text);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_init(const QJsonObject)));
}



void Comment::remove()
{
    auto url = QString("comments/%1.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);

    //connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readComment(const QJsonObject)));
}



void Comment::_init(const QJsonObject data)
{
//    qDebug() << data;

    _id             = data.value("id").toInt();
    _user           = new User(data.value("user").toObject(), this);
    _html           = data.value("comment_html").toString();
    _createdAt      = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    _isEditable     = data.value("can_edit").toBool();
    _isReportable   = data.value("can_report").toBool();
    _isDeletable    = data.value("can_delete").toBool();

    emit htmlChanged();
}



User::User(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _id         = data.value("id").toInt();
    _tlogUrl    = data.value("tlog_url").toString();
    _name       = data.value("name").toString();
    _slug       = data.value("slug").toString();

    auto userpic = data.value("userpic").toObject();

    _originalPic    = userpic.value("original_url").toString();
    _largePic       = userpic.value("large_url").toString();
    _thumb128       = userpic.value("thumb128_url").toString();
    _thumb64        = userpic.value("thumb64_url").toString();
    _symbol         = userpic.value("symbol").toString();
}



Rating::Rating(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entryId(0)
{
    _init(data);

    connect(Tasty::instance(), SIGNAL(ratingChanged(QJsonObject)), this, SLOT(_init(QJsonObject)));
}



void Rating::vote()
{
    if (!_isVotable)
        return;

    auto url = QString("entries/%1/votes.json").arg(_entryId);
    auto operation = (_isVoted ? QNetworkAccessManager::DeleteOperation
                               : QNetworkAccessManager::PostOperation);
    auto request = new ApiRequest(url, true, operation);

    connect(request, SIGNAL(success(const QJsonObject)),
            this, SLOT(_init(const QJsonObject)));
}



void Rating::_init(const QJsonObject data)
{
    int id = data.value("entry_id").toInt();
    if (id)
        _entryId    = id;

    _votes      = data.value("votes").toInt();
    _rating     = data.value("rating").toInt();
    _isVoted    = data.value("is_voted").toBool();
    _isVotable  = data.value("is_voteable").toBool();

    emit dataChanged();
}
