#include "feedmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../data/Entry.h"
#include "../data/Rating.h"



FeedModel::FeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _hasMore(true)
    , _loading(false)
    , _lastEntry(0)
    , _isPrivate(false)
    , _minRating(0)
    , _page(1)
    , _request(nullptr)
{
    qDebug() << "FeedModel";

    Q_TEST(connect(Tasty::instance()->settings(), SIGNAL(hideShortPostsChanged()),    this, SLOT(_changeHideSome())));
    Q_TEST(connect(Tasty::instance()->settings(), SIGNAL(hideNegativeRatedChanged()), this, SLOT(_changeHideSome())));

    Q_TEST(connect(Tasty::instance(), SIGNAL(authorized()), this, SLOT(_reloadRatings())));

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
    if (!_hasMore || _loading || parent.isValid() || (_mode == TlogMode && _tlog <= 0 && _slug.isEmpty()))
        return;

    qDebug() << "FeedModel::fetchMore";

    _loading = true;

    QString url = _url;
    if (_mode == TlogMode)
    {
        if (_tlog > 0)
            url = url.arg(_tlog);
        else if (!_slug.isEmpty())
            url = url.arg(_slug);
        else
        {
            qDebug() << "No tlog set in feed model";
            return;
        }
    }
    else if (_mode == BetterThanMode)
        url = url.arg(_minRating);

    if (!_query.isEmpty())
        url += QString("q=%1&page=%2&").arg(_query).arg(_page++);
    else if (_lastEntry)
        url += QString("since_entry_id=%1&").arg(_lastEntry);

    int limit = _entries.isEmpty() && _query.isEmpty() ? 10 : 20;
    url += QString("limit=%1").arg(limit);

    _request = new ApiRequest(url);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(error(int,QString)),   this, SLOT(_setPrivate(int))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)), this, SLOT(_setNotLoading(QObject*))));
}



void FeedModel::setMode(const FeedModel::Mode mode)
{
    reset(mode);
    if (mode == TlogMode && _tlog <= 0)
        _hasMore = false;
}



void FeedModel::setTlog(const int tlog)
{
    if (tlog > 0)
        reset(_mode, tlog);
}



void FeedModel::setSlug(const QString slug)
{
    if (!slug.isEmpty())
        reset(_mode, -1, slug);
}



void FeedModel::setMinRating(const int rating)
{
    if (rating > 0)
    {
        _minRating = rating;
        reset();
    }
}



void FeedModel::setQuery(const QString query)
{
    reset(InvalidMode, 0, QString(), query);
}



void FeedModel::reset(Mode mode, int tlog, QString slug, QString query)
{
    beginResetModel();

    if (mode != InvalidMode)
    {
        _mode = mode;
        _setUrl(mode);
    }

    if (tlog > 0)
        _tlog = tlog;

    if (!slug.isEmpty())
        _slug = slug;

    _page = 1;
    _query = QUrl::toPercentEncoding(query);
    emit queryChanged();

    _hasMore = true;
    _lastEntry = 0;
    _loading = false;

    if (_request)
        delete _request;
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



bool FeedModel::hideMode() const
{
    return (_mode == LiveMode || _mode == BestMode || _mode == ExcellentMode
            || _mode == GoodMode || _mode == WellMode || _mode == BetterThanMode);
}



bool FeedModel::hideShort() const
{
    return hideMode()
            && Tasty::instance()->settings()->hideShortPosts();
}



bool FeedModel::hideNegative() const
{
    return hideMode()
            && Tasty::instance()->settings()->hideNegativeRated();
}



void FeedModel::postText(const QString title, const QString content, FeedModel::Privacy privacy)
{
    if (_mode == AnonymousMode)
    {
        postAnonymous(title, content);
        return;
    }

    QString privacyValue;
    switch (privacy) {
    case Private:
        privacyValue = "private";
        break;
    case Public:
        privacyValue = "public";
        break;
    case Voting:
        privacyValue = "public_with_voting";
        break;
    }

    auto data = QString("title=%1&text=%2&privacy=%3")
            .arg(title)
            .arg(content)
            .arg(privacyValue);

    if (_mode == TlogMode)
        data += QString("&tlog_id=%1").arg(_tlog);

    qDebug() << data;

    auto request = new ApiRequest("entries/text.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addNewPost(QJsonObject))));
}



void FeedModel::postAnonymous(const QString title, const QString content)
{
    auto data = QString("title=%1&text=%2")
            .arg(title)
            .arg(content);

    qDebug() << data;

    auto request = new ApiRequest("entries/anonymous.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addNewPost(const QJsonObject))));
}



QHash<int, QByteArray> FeedModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "entry";
    return roles;
}



void FeedModel::_addItems(QJsonObject data)
{
    qDebug() << "FeedModel::_addItems";

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

    if (_lastEntry <= 0)
        _lastEntry = feed.last().toObject().value("id").toInt();

    QList<Entry*> all;
    all.reserve(feed.size());
    foreach(auto item, feed)
    {
        auto entry = new Entry(item.toObject(), this);
        all << entry;
    }

    bool loadMore = false;
    if (hideShort() || hideNegative())
        loadMore = _addSome(all);
    else
        _addAll(all);

    _loading = false;
    _request = nullptr;

    if (loadMore)
        fetchMore(QModelIndex());
}



void FeedModel::_addNewPost(QJsonObject data)
{
    auto entry = new Entry(data, this);

    beginInsertRows(QModelIndex(), 0, 0);
    _entries.prepend(entry);
    endInsertRows();

    emit entryCreated(entry);
}



void FeedModel::_changeHideSome()
{
    beginResetModel();

    if (_allEntries.isEmpty())
        _allEntries = _entries;

    _entries.clear();

    bool s = hideShort();
    bool n = hideNegative();

    foreach (auto e, _allEntries)
        if ((!s || e->wordCount() >= 100)
            && (!n || e->rating()->bayesRating() >= 0))
            _entries << e;

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



void FeedModel::_reloadRatings()
{
    auto entries = _allEntries.isEmpty() ? _entries : _allEntries;
    if (entries.isEmpty())
        return;

    QString url("ratings.json?ids=");
    url.reserve(entries.size() * 9 + 20);
    for (int i = 0; i < entries.size() - 1; i++)
        url += QString("%1,").arg(entries.at(i)->entryId());
    url += QString::number(entries.last()->entryId());

    auto request = new ApiRequest(url);
    Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_setRatings(QJsonArray))));
}



void FeedModel::_setRatings(const QJsonArray data)
{
    auto entries = _allEntries.isEmpty() ? _entries : _allEntries;
    if (entries.isEmpty())
        return;

    foreach (auto rating, data)
    {
        auto id = rating.toObject().value("entry_id").toInt();
        foreach (auto entry, entries) //! \todo optimize
            if (entry->entryId() == id)
            {
                entry->rating()->init(rating.toObject());
                break;
            }
    }
}



void FeedModel::_addAll(QList<Entry*>& all)
{
    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + all.size() - 1);
    _entries << all;
    endInsertRows();
}



bool FeedModel::_addSome(QList<Entry*>& all)
{
    bool s = hideShort();
    bool n = hideNegative();

    _allEntries << all;

    QList<Entry*> some;
    foreach (auto e, all)
        if ((!s || e->wordCount() >= 100)
            && (!n || e->rating()->bayesRating() >= 0))
            some << e;

    if (some.isEmpty())
        return true;

    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + some.size() - 1);
    _entries << some;
    endInsertRows();

    return false;
}



void FeedModel::_setUrl(FeedModel::Mode mode)
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
    case BetterThanMode:
        _url = "feeds/best.json?rating=%1&";
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
}
