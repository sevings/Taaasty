#include "feedmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "defines.h"

#include "tasty.h"
#include "apirequest.h"
#include "datastructures.h"



FeedModel::FeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _hasMore(true)
    , _loading(false)
    , _lastEntry(0)
    , _isPrivate(false)
    , _request(nullptr)
{
    Q_TEST(connect(Tasty::instance()->settings(), SIGNAL(hideShortPostsChanged()), this, SLOT(_changeHideShort())));

    setMode(LiveMode);
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

    _request = new ApiRequest(url);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(error(int,QString)),   this, SLOT(_setPrivate(int))));
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

    reset(mode);
    if (mode == TlogMode && _tlog <= 0)
        _hasMore = false;
}



void FeedModel::setTlog(const int tlog)
{
    if (tlog > 0)
        reset(_mode, tlog);
}



void FeedModel::reset(Mode mode, int tlog)
{
    beginResetModel();

    if (mode != InvalidMode)
        _mode = mode;

    if (tlog >= 0)
        _tlog = tlog;

    _hasMore = true;
    _lastEntry = 0;
    _loading = false;
    _request = nullptr;

    if (_allEntries.isEmpty())
        qDeleteAll(_entries);
    else
        qDeleteAll(_allEntries);

    _entries.clear();
    _allEntries.clear();

    endResetModel();

    emit hasMoreChanged();
}



bool FeedModel::hideShort() const
{
    return (_mode == LiveMode || _mode == BestMode || _mode == ExcellentMode
            || _mode == GoodMode || _mode == WellMode || _mode == BetterThanMode)
            && Tasty::instance()->settings()->hideShortPosts();
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
        _request = nullptr;
        return;
    }

    for (int i = feed.size() - 1; i >= 0; i--)
        if (feed.at(i).toObject().value("fixed_state").toString() != "fixed")
        {
            _lastEntry = feed.at(i).toObject().value("id").toInt();
            break;
        }

    QList<Entry*> all;
    all.reserve(feed.size());
    foreach(auto item, feed)
    {
        auto entry = new Entry(item.toObject(), this);
        all << entry;
    }

    bool loadMore = false;
    if (hideShort())
        loadMore = _addLonger(all);
    else
        _addAll(all);

    _loading = false;
    _request = nullptr;

    if (loadMore)
        fetchMore(QModelIndex());
}



void FeedModel::_changeHideShort()
{
    beginResetModel();

    if (hideShort())
    {
        _entries.swap(_allEntries);
        foreach(auto e, _allEntries)
            if (e->wordCount() >= 100)
                _entries << e;
    }
    else if (!_allEntries.isEmpty()) // prev hide
    {
        _entries.clear();
        _entries.swap(_allEntries);
    }

    endResetModel();
}



void FeedModel::_setPrivate(int errorCode)
{
    if (errorCode == 403)
    {
        _isPrivate = true;
        emit isPrivateChanged();
    }
}



void FeedModel::_setNotLoading(QObject* request)
{
    if (request == _request)
    {
        _loading = false;
        _request = nullptr;
    }
}



void FeedModel::_addAll(QList<Entry*>& all)
{
    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + all.size() - 1);
    _entries << all;
    endInsertRows();
}



bool FeedModel::_addLonger(QList<Entry*>& all)
{
    _allEntries << all;

    QList<Entry*> longer;
    foreach (auto e, all)
        if (e->wordCount() >= 100)
            longer << e;

    if (longer.isEmpty())
        return true;

    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + longer.size() - 1);
    _entries << longer;
    endInsertRows();

    return false;
}
