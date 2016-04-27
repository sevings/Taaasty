#include "feedmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "apirequest.h"
#include "datastructures.h"



FeedModel::FeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _hasMore(true)
    , _loading(false)
    , _lastEntry(0)
{
    setMode(LiveMode);
}



FeedModel::~FeedModel()
{
//    qDeleteAll(_entries);
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

    if (role == Qt::UserRole)
        return QVariant::fromValue<Entry*>(_entries.at(index.row()));

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
    QString url = _url;
    if (_mode == TlogMode)
        url = url.arg(_tlog);
    url += QString("limit=%1").arg(limit);
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
    if (tlog <= 0)
        return;

    beginResetModel();

    _tlog = tlog;
    _hasMore = true;
    _lastEntry = 0;
    _loading = false;
    qDeleteAll(_entries);

    endResetModel();

    emit hasMoreChanged();
}



QHash<int, QByteArray> FeedModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "entry";
    return roles;
}



void FeedModel::_addItems(QJsonObject data)
{
//    qDebug() << "adding entries";

    auto feed = data.value("entries").toArray();
    if (feed.isEmpty())
    {
        _hasMore = false;
        emit hasMoreChanged();
        _loading = false;
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
    foreach(auto item, feed)
    {
        auto entry = new Entry(item.toObject(), this);
        _entries << entry;
    }

    endInsertRows();

    _loading = false;
}
