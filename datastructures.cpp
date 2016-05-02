#include "datastructures.h"

#include "tasty.h"
#include "apirequest.h"
#include "commentsmodel.h"
#include "attachedimagesmodel.h"



Entry::Entry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _commentsModel(nullptr)
    , _attachedImagesModel(nullptr)
{
    _init(data);
}



void Entry::setId(const int id)
{
    if (id <= 0)
        return;

    _id = id;

    auto request = new ApiRequest(QString("entries/%1.json").arg(_id));
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));
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
    _title           = data.value("title").toString();
    _truncatedTitle  = data.value("title_truncated").toString();
    _text            = data.value("text").toString();
    _truncatedText   = data.value("text_truncated").toString();
    _source          = data.value("source").toString();
    _imagePreview    = data.value("preview_image").toObject();

    delete _commentsModel;
    _commentsModel = new CommentsModel(this);

    auto imageAttach = data.value("image_attachments").toArray();
    delete _attachedImagesModel;
    _attachedImagesModel = new AttachedImagesModel(&imageAttach, this);

    emit updated();
    emit commentsCountChanged();
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



AttachedImage::AttachedImage(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _type = data.value("content_type").toString().split("/").last();

    auto image = data.value("image").toObject();
    _url = image.value("url").toString();

    auto geometry = image.value("geometry").toObject();
    _width = geometry.value("width").toInt();
    _height = geometry.value("height").toInt();
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
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
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



Author::Author(const QJsonObject data, QObject *parent)
    : User(data, parent)
{
    _isFemale  = data.value("is_female").toBool();
    _isPrivacy = data.value("is_privacy").toBool();
    _isOnline  = data.value("is_online").toBool();
    _isFlow    = data.value("is_flow").toBool();
    _title     = data.value("title").toString();

    _entriesCount = Tasty::num2str(data.value("total_entries_count").toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value("public_entries_count").toInt(),
                                         "открытая запись", "открытые записи", "открытых записей");
    _privateEntriesCount = Tasty::num2str(data.value("private_entries_count").toInt(),
                                          "скрытая запись", "скрытые записи", "скрытых записей");

    auto date = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    auto today = QDateTime::currentDateTime();
    int days = (today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000);
    _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");

    _followingsCount = Tasty::num2str(data.value("followings_count").toInt(), "подписка", "подписки", "подписок");
}



Tlog::Tlog(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);
}



void Tlog::setId(const int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;

    auto request = new ApiRequest(QString("tlog/%1.json").arg(_id));
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));
}



void Tlog::_init(const QJsonObject data)
{
    _id = data.value("id").toInt();
    _title = data.value("title").toString();
    _entriesCount = Tasty::num2str(data.value("total_entries_count").toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value("public_entries_count").toInt(),
                                         "открытая запись", "открытые записи", "открытых записей");
    _privateEntriesCount = Tasty::num2str(data.value("private_entries_count").toInt(),
                                          "скрытая запись", "скрытые записи", "скрытых записей");

    auto date = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    auto today = QDateTime::currentDateTime();
    int days = (today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000);
    _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");

    auto relations = data.value("relationships_summary").toObject();
    _followersCount = Tasty::num2str(relations.value("followers_count").toInt(), "подписчик", "подписчика", "подписчиков");
    _followingsCount = Tasty::num2str(relations.value("followings_count").toInt(), "подписка", "подписки", "подписок");
    _ignoredCount = Tasty::num2str(relations.value("ignored_count").toInt(), "блокирован", "блокировано", "блокировано");

    _isFollowingMe = data.value("his_relationship").toString() == "friend";
    _amIFollowing = data.value("my_relationship").toString() == "friend";

    _author = new Author(data.value("author").toObject(), this);

    emit updated();
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



Notification::Notification(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _id         = data.value("id").toInt();
    _createdAt  = Tasty::parseDate(data.value("created_at").toString());
    _sender     = new User(data.value("sender").toObject(), this);
    _read       = !data.value("read_at").isNull();
    _action     = data.value("action").toString();
    _actionText = data.value("action_text").toString();
    _text       = data.value("text").toString();
    _entityId   = data.value("entity_id").toInt();
    _entityType = data.value("entity_type").toString();
    _parentId   = data.value("parent_id").toInt();
    _parentType = data.value("parent_type").toString();
}
