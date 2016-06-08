#include "datastructures.h"

#include <QRegularExpression>
#include <QUrl>

#include "defines.h"

#include "tasty.h"
#include "apirequest.h"
#include "commentsmodel.h"
#include "attachedimagesmodel.h"
#include "bayes.h"



CalendarEntry::CalendarEntry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entry(nullptr)
{
    _id              = data.value("id").toInt();
    _createdAt       = Tasty::parseDate(data.value("created_at").toString());
    _url             = data.value("entry_url").toString();
    _type            = data.value("type").toString();
    _isPrivate       = data.value("is_private").toBool();
    _commentsCount   = data.value("comments_count").toInt();
    _truncatedTitle  = data.value("title_truncated").toString();
    _isFlow          = data.value("is_flow").toBool();
}



Entry* CalendarEntry::full()
{
    if (_entry)
        return _entry;

    _entry = new Entry(QJsonObject(), this);
    _entry->setId(_id);
    return _entry;
}



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
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));

    _loading = true;
    emit loadingChanged();
}



void Entry::addComment(const QString text)
{
    auto content = QUrl::toPercentEncoding(text.trimmed());
    auto data    = QString("entry_id=%1&text=%2").arg(_id).arg(QString::fromUtf8(content));
    auto request = new ApiRequest("comments.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    qDebug() << data;

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(commentAdded(const QJsonObject))));
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

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeWatched(QJsonObject))));
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

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_changeFavorited(QJsonObject))));
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

    QRegularExpression re("\\s[^\\s]+\\s");
    _wordCount       = _title.count(re) + _text.count(re);

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



void Entry::_correctHtml()
{
    Tasty::correctHtml(_title);
    Tasty::correctHtml(_text);

    emit htmlUpdated();
}

Rating* Entry::rating() const
{
    return _rating;
}



int Entry::wordCount() const
{
    return _wordCount;
}



AttachedImage::AttachedImage(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    auto image = data.value("image").toObject();
    _url = image.value("url").toString();

    auto type = data.value("content_type").toString();
    _type = type.split("/").last();

    auto geometry = image.value("geometry").toObject();
    _width = geometry.value("width").toInt();
    _height = geometry.value("height").toInt();
}



AttachedImage::AttachedImage(const QJsonObject data, Media* parent)
    : QObject(parent)
{
    _url = data.value("href").toString();

    auto type = data.value("type").toString();
    _type = type.split("/").last();

    auto geometry = data.value("media").toObject();
    _width = geometry.value("width").toInt();
    _height = geometry.value("height").toInt();
}



Media::Media(const QJsonObject data, QObject* parent)
    : QObject(parent)
{
    auto meta = data.value("meta").toObject();
    _title    = meta.value("title").toString();
    _duration = meta.value("duration").toInt();

    auto links = data.value("links").toObject();
    _url = links.value("player").toArray().first().toObject().value("href").toString();

    auto thumb = links.value("thumbnail").toArray().first().toObject();
    _thumbnail = new AttachedImage(thumb, this);

    auto icon = links.value("icon").toArray().first().toObject();
    _icon     = new AttachedImage(icon, this);
}



Comment::Comment(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Comment::Comment(const Notification* data, QObject* parent)
    : QObject(parent)
{
    _id             = data->_entityId;
    _user           = data->_sender;
    _html           = data->_text;
    _createdAt      = Tasty::parseDate(data->_createdAt);
    _isEditable     = false;
    _isReportable   = false;
    _isDeletable    = false;

    auto entryId = data->_parentId;
    auto url = QString("comments.json?entry_id=%1&from_comment_id=%2&limit=1").arg(entryId).arg(_id - 1);
    auto request = new ApiRequest(url);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_update(const QJsonObject))));
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

    connect(request, SIGNAL(success(const QString)), this, SLOT(_remove(const QString)));
}



void Comment::_init(const QJsonObject data)
{
    _id             = data.value("id").toInt();
    _user           = new User(data.value("user").toObject(), this);
    _html           = data.value("comment_html").toString();
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
    _isEditable     = data.value("can_edit").toBool();
    _isReportable   = data.value("can_report").toBool();
    _isDeletable    = data.value("can_delete").toBool();

    _correctHtml();

    emit updated();
}



void Comment::_update(const QJsonObject data)
{
    auto list = data.value("comments").toArray();
    if (list.isEmpty())
        return;

    _init(list.first().toObject());
}



void Comment::_correctHtml()
{
    Tasty::correctHtml(_html, false);

    emit htmlUpdated();
}



void Comment::_remove(const QString data)
{
    if (data == "true")
        deleteLater();
    else
        qDebug() << "removing commment:" << data;
}



User::User(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);
}

int User::id() const
{
    return _id;
}

QString User::name() const
{
    return _name;
}

QString User::slug() const
{
    return _slug;
}



void User::_init(const QJsonObject data)
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

    auto colors = userpic.value("default_colors").toObject();

    _backgroundColor = colors.value("background").toString();
    _nameColor       = colors.value("name").toString();

    emit updated();
}



Author::Author(const QJsonObject data, QObject *parent)
    : User(data, parent)
{
    _init(data);
}

bool Author::isFemale() const
{
    return _isFemale;
}

bool Author::isFlow() const
{
    return _isFlow;
}

bool Author::isPremium() const
{
    return _isPremium;
}

bool Author::isDaylog() const
{
    return _isDaylog;
}



void Author::checkStatus()
{
    auto url = QString("online_statuses.json?user_ids=%1").arg(id());
    auto request = new ApiRequest(url);

    Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_initStatus(QJsonArray))));
}



void Author::_init(const QJsonObject data)
{
    User::_init(data);

    _isFemale  = data.value("is_female").toBool();
    _isPrivacy = data.value("is_privacy").toBool();
    _isFlow    = data.value("is_flow").toBool();
    _isPremium = data.value("is_premium").toBool();
    _isDaylog  = data.value("is_daylog").toBool();
    _title     = data.value("title").toString();

    _entriesCount = Tasty::num2str(data.value("total_entries_count").toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value("public_entries_count").toInt(),
                                         "запись", "записи", "записей");
    _privateEntriesCount = Tasty::num2str(data.value("private_entries_count").toInt(),
                                          "скрытая запись", "скрытые записи", "скрытых записей");

    auto date = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    auto today = QDateTime::currentDateTime();
    int days = qRound(double(today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000));
    _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");

    _followingsCount = Tasty::num2str(data.value("followings_count").toInt(), "подписка", "подписки", "подписок");

    emit updated();

    _initStatus(data);
}



void Author::_initStatus(const QJsonArray data)
{
    if (data.size())
        _initStatus(data.first().toObject());
}



void Author::_initStatus(const QJsonObject data)
{
    _isOnline  = data.value("is_online").toBool();
    auto last = data.value("last_seen_at").toString();
    if (_isOnline)
        _lastSeenAt = "Онлайн";
    else if (last.isEmpty())
        _lastSeenAt = "Не в сети";
    else
        _lastSeenAt = QString("Был%1 в сети %2").arg(_isFemale ? "а" : "")
                .arg(Tasty::parseDate(last, false));

    emit statusUpdated();
}



Tlog::Tlog(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _author(nullptr)
    , _loading(false)
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

    _loading = true;
    emit loadingChanged();
}



void Tlog::setSlug(const QString slug)
{
    if (slug.isEmpty() || slug == _slug)
        return;

   _slug = slug;

    auto request = new ApiRequest(QString("tlog/%1.json").arg(_slug));
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));

    _loading = true;
    emit loadingChanged();
}



void Tlog::_init(const QJsonObject data)
{
    _id = data.value("id").toInt();
    _slug = data.value("slug").toString();
    _title = data.value("title").toString();
    _entriesCount = Tasty::num2str(data.value("total_entries_count").toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value("public_entries_count").toInt(),
                                         "запись", "записи", "записей");
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
    auto my = data.value("my_relationship").toString();
    _amIFollowing  = my == "friend";
    _isMe         = my.isEmpty();

    auto authorData = data.value("author").toObject();
    if (_author)
        _author->_init(authorData);
    else
        _author = new Author(authorData, this);

    emit updated();

    _loading = false;
    emit loadingChanged();
}



Rating::Rating(const QJsonObject data, Entry* parent)
    : QObject(parent)
    , _entryId(0)
    , _bayesRating(0)
    , _isBayesVoted(false)
    , _isVotedAgainst(false)
    , _parent(parent)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(ratingChanged(QJsonObject)), this, SLOT(_init(QJsonObject))));
}



void Rating::reCalcBayes()
{
    auto type = Bayes::instance()->entryVoteType(_parent);
    if (type == Bayes::Water)
    {
        _isBayesVoted = false;
        _isVotedAgainst = true;
    }
    else if (type == Bayes::Fire)
    {
        _isBayesVoted = true;
        _isVotedAgainst = false;
    }
    else
    {
        _isBayesVoted = false;
        _isVotedAgainst = false;

        if (_isVoted)
        {
            voteBayes();
            return;
        }
    }

    _bayesRating = Bayes::instance()->classify(_parent);
    emit bayesChanged();
}



void Rating::vote()
{
    voteBayes();

    if (!_isVotable || !Tasty::instance()->isAuthorized())
        return;

    auto url = QString("entries/%1/votes.json").arg(_entryId);
    auto operation = (_isVoted ? QNetworkAccessManager::DeleteOperation
                               : QNetworkAccessManager::PostOperation);
    auto request = new ApiRequest(url, true, operation);

    connect(request, SIGNAL(success(const QJsonObject)),
            this, SLOT(_init(const QJsonObject)));
}



void Rating::voteBayes()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    _bayesRating = Bayes::instance()->voteForEntry(_parent, Bayes::Fire);
    _isBayesVoted = true;

    emit bayesChanged();

    qDebug() << "voted";
}



void Rating::voteAgainst()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    _bayesRating = Bayes::instance()->voteForEntry(_parent, Bayes::Water);
    _isVotedAgainst = true;

    emit bayesChanged();
}



void Rating::_init(const QJsonObject data)
{
    int id = data.value("entry_id").toInt();
    if (id)
        _entryId = id;

    _votes      = data.value("votes").toInt();
    _rating     = data.value("rating").toInt();
    _isVoted    = data.value("is_voted").toBool();
    _isVotable  = data.value("is_voteable").toBool();

    emit dataChanged();
}

int Rating::bayesRating() const
{
    return _bayesRating;
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

int Notification::entityId() const
{
    return _entityId;
}
