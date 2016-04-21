#include "feedmodel.h"

#include <QDebug>

#include "apirequest.h"



FeedModel::FeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _hasMore(true)
    , _loading(false)
    , _lastEntry(0)
{
    setMode(LiveMode);

    connect(Tasty::instance(), SIGNAL(ratingChanged(int,QJsonObject)),
            this, SLOT(_changeRating(int,QJsonObject)));
}



FeedModel::~FeedModel()
{
    qDeleteAll(_entries);
}



int FeedModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _entries.size();
}



QVariant FeedModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _entries.size())
        return QVariant();

    const auto entry = _entries.at(index.row());
    switch (role)
    {
    case IdRole:
        return entry->id;
    case CreatedAtRole:
        return entry->createdAt;
    case UrlRole:
        return entry->url;
    case TypeRole:
        return entry->type;
    case VotableRole:
        return entry->isVotable;
    case PrivateRole:
        return entry->isPrivate;
    case TlogRole:
        return entry->tlog;
    case AuthorRole:
        return entry->author;
    case RatingRole:
        return entry->rating;
    case CommentsCountRole:
        return entry->commentsCount;
    case TitleRole:
        return entry->title;
    case TruncatedTitleRole:
        return entry->truncatedTitle;
    case TextRole:
        return entry->text;
    case TruncatedTextRole:
        return entry->truncatedText;
    case ImageAttachRole:
        return entry->imageAttach;
    case ImagePreviewRole:
        return entry->imagePreview;
    }

    qDebug() << "role" << role;

    return QVariant();
}



bool FeedModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _hasMore;
}



void FeedModel::fetchMore(const QModelIndex& parent)
{
//    qDebug() << "fetch more";

    if (!_hasMore || _loading || parent.isValid() || (_mode == TlogMode && !_tlog))
        return;

    _loading = true;

    int limit = _entries.isEmpty() ? 10 : 20;
    QString url = _url + QString("limit=%1").arg(limit);
    if (_lastEntry)
        url += QString("&since_entry_id=%1").arg(_lastEntry);

    auto request = new ApiRequest(url);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));
}



void FeedModel::setMode(const FeedModel::Mode mode)
{
    switch(mode)
    {
    case MyTlogMode:
        _url = "my_feeds/tlog.json?";
        break;
    case FriendsMode:
        _url = "my_feeds/friends.json?";
        break;
    case LiveMode:
        _url = "feeds/live.json?";
        break;
    case AnonymousMode:
        _url = "feeds/anonymous.json?";
        break;
    case BestMode:
        _url = "feeds/best.json?rating=best&";
        break;
    case ExcellentMode:
        _url = "feeds/best.json?rating=excellent&";
        break;
    case WellMode:
        _url = "feeds/best.json?rating=well&";
        break;
    case GoodMode:
        _url = "feeds/best.json?rating=good&";
        break;
    case TlogMode:
        _url = "tlog/%1/entries.json?";
        break;
    case FavoritesMode:
        _url = "my_feeds/favorites.json?";
        break;
    default:
        qDebug() << "feed mode =" << mode;
    }

    _mode = mode;
}



void FeedModel::setTlog(const int tlog)
{
    if (tlog > 0)
        _tlog = tlog;
}



QHash<int, QByteArray> FeedModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[IdRole]              = "id";
    roles[CreatedAtRole]       = "createdAt";
    roles[UrlRole]             = "url";
    roles[TypeRole]            = "type";
    roles[VotableRole]         = "isVotable";
    roles[PrivateRole]         = "isPrivate";
    roles[TlogRole]            = "tlog";
    roles[AuthorRole]          = "author";
    roles[RatingRole]          = "rating";
    roles[CommentsCountRole]   = "commentsCount";
    roles[TitleRole]           = "title";
    roles[TruncatedTitleRole]  = "truncatedTitle";
    roles[TextRole]            = "text";
    roles[TruncatedTextRole]   = "truncatedText";
    roles[ImageAttachRole]     = "imageAttach";
    roles[ImagePreviewRole]    = "imagePreview";

    return roles;
}



void FeedModel::_addItems(QJsonObject data)
{
//    qDebug() << "adding entries";

    auto feed = data.value("entries").toArray();
    if (feed.isEmpty())
    {
        _hasMore = false;
        return;
    }

    for (int i = feed.size() - 1; i >= 0; i--)
        if (feed.at(i).toObject().value("fixed_state").toString() != "fixed")
        {
            _lastEntry = feed.at(i).toObject().value("id").toInt();
            break;
        }

    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + feed.size() - 1);

    _entries.reserve(_entries.size() + feed.size());
    int row = _entries.size();
    foreach(auto item, feed)
    {
        auto entry = _createEntry(item.toObject());
        _entries << entry;

        _entriesById[entry->id] = entry;

        entry->row = row;
        row++;
    }

    endInsertRows();

    _loading = false;
}



void FeedModel::_changeRating(const int entryId, const QJsonObject rating)
{
    if (!_entriesById.contains(entryId))
        return;

    auto entry = _entriesById.value(entryId);
    entry->rating = rating;

    auto row = entry->row;
    emit dataChanged(index(row), index(row), QVector<int>(1, RatingRole));
}



FeedModel::Entry *FeedModel::_createEntry(QJsonObject data)
{
    auto entry = new Entry;

    entry->id = data.value("id").toInt();
    entry->createdAt = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    entry->url = data.value("url").toString();
    entry->type = data.value("type").toString();
    entry->isVotable = data.value("is_voteable").toBool();
    entry->isPrivate = data.value("is_private").toBool();
    entry->tlog = data.value("tlog").toObject();
    entry->author = data.value("author").toObject();
    entry->rating = data.value("rating").toObject();
    entry->commentsCount = data.value("comments_count").toInt();
    entry->title = data.value("title").toString();
    entry->truncatedTitle = data.value("title_truncated").toString();
    entry->text = data.value("text").toString();
    entry->truncatedText = data.value("text_truncated").toString();
    entry->imageAttach = data.value("image_attachments").toArray();
    entry->imagePreview = data.value("preview_image").toObject();

    return entry;
}
