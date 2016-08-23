#include "feedmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>
#include <QQmlEngine>

#include "../defines.h"

#include "../tasty.h"
#include "../pusherclient.h"
#include "../settings.h"
#include "../apirequest.h"

#include "../data/Entry.h"
#include "../data/Rating.h"



FeedModel::FeedModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _mode(InvalidMode)
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
}



FeedModel::~FeedModel()
{
    _clear();
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
    {
        auto id = _entries.at(index.row());
        return QVariant::fromValue<Entry*>(_entry(id));
    }

    qDebug() << "role" << role;

    return QVariant();
}



bool FeedModel::canFetchMore(const QModelIndex& parent) const
{
    if (_mode == InvalidMode || parent.isValid())
        return false;

    return _hasMore;
}



void FeedModel::fetchMore(const QModelIndex& parent)
{
    if (!_hasMore || _loading || parent.isValid()
            || (_mode == TlogMode && _tlog <= 0 && _slug.isEmpty())
            || _mode == InvalidMode)
        return;

    qDebug() << "FeedModel::fetchMore";

    _loading = true;
    emit loadingChanged();

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

    auto splitter = url.endsWith(".json") ? "?" : "&";

    if (!_query.isEmpty())
        url += QString("%1q=%2&page=%3").arg(splitter).arg(_query).arg(_page++);
    else if (!_prevDate.isEmpty())
        url += QString("%1date=%2").arg(splitter).arg(_prevDate);
    else if (_lastEntry)
        url += QString("%1since_entry_id=%2").arg(splitter).arg(_lastEntry);

    if (_prevDate.isEmpty())
    {
        splitter = url.endsWith(".json") ? "?" : "&";
        int limit = _entries.isEmpty() && _query.isEmpty() ? 10 : 20;
        url += QString("%1limit=%2").arg(splitter).arg(limit);
    }

    _request = new ApiRequest(url);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(error(int,QString)),   this, SLOT(_setPrivate(int))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)), this, SLOT(_setNotLoading(QObject*))));
}



void FeedModel::setMode(const FeedModel::Mode mode)
{
    reset(mode);
    if (mode == TlogMode && _tlog <= 0 && _slug.isEmpty())
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

    _prevDate.clear();

    _hasMore = true;
    _lastEntry = 0;
    _loading = false;
    emit loadingChanged();

    delete _request;
    _request = nullptr;

    _clear();

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



bool FeedModel::hasMore() const
{
    return _hasMore;
}



bool FeedModel::loading() const
{
    return _loading;
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

    auto request = new ApiRequest("v1/entries/text.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addNewPost(QJsonObject))));
}



void FeedModel::postAnonymous(const QString title, const QString content)
{
    auto data = QString("title=%1&text=%2")
            .arg(title)
            .arg(content);

    qDebug() << data;

    auto request = new ApiRequest("v1/entries/anonymous.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addNewPost(const QJsonObject))));
}



void FeedModel::setSinceEntryId(int id)
{
    if (id > 0)
        _lastEntry = id + 1;
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

    _request = nullptr;

    auto feed =  data.contains("items") ? data.value("items").toArray()
                                        : data.value("entries").toArray();
    if (feed.isEmpty())
    {
        _hasMore = false;
        emit hasMoreChanged();
        _loading = false;
        emit loadingChanged();
        return;
    }

    if (data.contains("prev_date"))
    {
        auto prev = data.value("prev_date");
        if (prev.isNull())
        {
            _hasMore = false;
            emit hasMoreChanged();
            _prevDate.clear();
        }
        else
            _prevDate = prev.toString();
    }

    QList<Entry*> all;
    all.reserve(feed.size());
    foreach(auto item, feed)
    {
        auto obj = item.toObject();
        auto json = obj.contains("entry") ? obj.value("entry").toObject()
                                          : obj;
        auto id = json.value("id").toInt();
        auto entry = Tasty::instance()->pusher()->entry(id);
        if (!entry)
            entry = new Entry(json, this);

        all << entry;
    }

    for (int i = all.size() - 1; i >= 0; i--)
        if (!all.at(i)->isFixed())
        {
            _lastEntry = all.at(i)->entryId();
            break;
        }

    if (_lastEntry <= 0)
        _lastEntry = all.last()->entryId();

    bool loadMore = false;
    if (hideShort() || hideNegative())
        loadMore = _addSome(all);
    else
        _addAll(all);

    _loading = false;

    if (loadMore)
        fetchMore(QModelIndex());
    else
        emit loadingChanged();
}



void FeedModel::_addNewPost(QJsonObject data)
{
    auto entry = new Entry(data, this);

    beginInsertRows(QModelIndex(), 0, 0);
    _entries.prepend(entry->entryId());
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
        if ((!s || _entry(e)->wordCount() >= 100)
            && (!n || _entry(e)->rating()->bayesRating() >= 0))
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
    if (request != _request)
        return;

    if (_loading)
    {
        _loading = false;
        emit loadingChanged();
    }

    _request = nullptr;
}



void FeedModel::_reloadRatings()
{
    if (_mode == FriendsMode)
    {
        reset();
        return;
    }

    auto entries = _allEntries.isEmpty() ? _entries : _allEntries;
    if (entries.isEmpty())
        return;

    QString url("v1/ratings.json?ids=");
    url.reserve(entries.size() * 9 + 20);
    for (int i = 0; i < entries.size() - 1; i++)
        url += QString("%1,").arg(entries.at(i));
    url += QString::number(entries.last());

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
            if (entry == id)
            {
                _entry(entry)->rating()->init(rating.toObject());
                break;
            }
    }
}



void FeedModel::_addAll(QList<Entry*>& all)
{
    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + all.size() - 1);
    foreach (auto e, all)
        _entries << e->entryId();
    endInsertRows();
}



bool FeedModel::_addSome(QList<Entry*>& all)
{
    bool s = hideShort();
    bool n = hideNegative();

    QList<int> some;
    foreach (auto e, all)
    {
        _allEntries << e->entryId();

        if ((!s || e->wordCount() >= 100)
            && (!n || e->rating()->bayesRating() >= 0))
            some << e->entryId();
    }

    if (some.isEmpty())
        return true;

    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + some.size() - 1);
    _entries << some;
    endInsertRows();

    return false;
}



void FeedModel::_clear()
{
    auto entries = _allEntries.isEmpty() ? _entries : _allEntries;
    foreach (auto e, entries)
    {
        auto entry = Tasty::instance()->pusher()->entry(e);
        if (!entry)
            continue;

//        if ((!entry->parent()
//             && QQmlEngine::objectOwnership(entry) == QQmlEngine::CppOwnership)
//                || entry->parent() == this)
//            delete entry;
    }

    _entries.clear();
    _allEntries.clear();

}



void FeedModel::_setUrl(FeedModel::Mode mode)
{
    switch(mode)
    {
    case MyTlogMode:
        _url = QString("v1/tlog/%1/entries/tlogs.json")
                .arg(Tasty::instance()->settings()->login());
        break;
    case FriendsMode:
        _url = "v1/my_feeds/friends/tlogs.json";
        break;
    case LiveMode:
        _url = "v1/feeds/live/tlogs.json";
        break;
    case AnonymousMode:
        _url = "v1/feeds/anonymous/tlogs.json";
        break;
    case BestMode:
        _url = "v1/feeds/best/tlogs.json?rating=best";
        break;
    case ExcellentMode:
        _url = "v1/feeds/best/tlogs.json?rating=excellent";
        break;
    case WellMode:
        _url = "v1/feeds/best/tlogs.json?rating=well";
        break;
    case GoodMode:
        _url = "v1/feeds/best/tlogs.json?rating=good";
        break;
    case BetterThanMode:
        _url = "v1/feeds/best/tlogs.json?rating=%1";
        break;
    case TlogMode:
        _url = "v1/tlog/%1/entries/tlogs.json";
        break;
    case FavoritesMode:
        _url = QString("v1/tlog/%1/favorites/tlogs.json")
                .arg(Tasty::instance()->settings()->login());
        break;
    default:
        qDebug() << "feed mode =" << mode;
    }
}



Entry*FeedModel::_entry(int id) const
{
    auto e = Tasty::instance()->pusher()->entry(id);
    if (e)
        return e;

    e = new Entry();
    e->setId(id);
    return e;
}
