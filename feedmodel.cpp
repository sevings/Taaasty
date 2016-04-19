#include "feedmodel.h"

#include <QDateTime>
#include <QDebug>
#include <QJsonArray>

#include "apirequest.h"



FeedModel::FeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _hasMore(true)
    , _loading(false)
    , _lastEntry(0)
{
    setMode(LiveMode);
}



int FeedModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _items.size();
}



QVariant FeedModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _items.size())
        return QVariant();

    const auto entry = _items.at(index.row());
    switch (role)
    {
    case IdRole:
        return entry.value("id").toInt();
    case CreatedAtRole:
        return QDateTime::fromString(entry.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    case UrlRole:
        return entry.value("url").toString();
    case TypeRole:
        return entry.value("type").toString();
    case VotableRole:
        return entry.value("is_voteable").toBool();
    case PrivateRole:
        return entry.value("is_private").toBool();
    case TlogRole:
        return entry.value("tlog").toObject();
    case AuthorRole:
        return entry.value("author").toObject();
    case RatingRole:
        return entry.value("rating").toObject();
    case CommentsCountRole:
        return entry.value("comments_count").toInt();
    case TitleRole:
        return entry.value("title").toString();
    case TruncatedTitleRole:
        return entry.value("title_truncated").toString();
    case TextRole:
        return entry.value("text").toString();
    case TruncatedTextRole:
        return entry.value("text_truncated").toString();
    case ImageAttachRole:
        return entry.value("image_attachments").toArray();
    case ImagePreviewRole:
        return entry.value("preview_image").toObject();
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

    int limit = _items.isEmpty() ? 10 : 20;
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

    _lastEntry = feed.last().toObject().value("id").toInt();

    beginInsertRows(QModelIndex(), _items.size(), _items.size() + feed.size() - 1);

    _items.reserve(_items.size() + feed.size());
    foreach(auto entry, feed)
        _items << entry.toObject();

    endInsertRows();

    _loading = false;
}
