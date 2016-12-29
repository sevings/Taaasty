// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "feedmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "../tasty.h"
#include "../tastydatacache.h"
#include "../settings.h"
#include "../apirequest.h"

#include "../data/Rating.h"
#include "../data/User.h"
#include "../data/Tlog.h"
#include "../data/Flow.h"



FeedModel::FeedModel(QObject* parent)
    : TastyListModel(parent)
    , _fixedCount(0)
    , _allFixedCount(0)
    , _tlog(new Tlog(this))
    , _mode(InvalidMode)
    , _lastEntry(0)
    , _minRating(0)
    , _page(1)
{
    qDebug() << "FeedModel";

    Q_TEST(connect(Tasty::instance()->settings(), &Settings::hideShortPostsChanged,    this, &FeedModel::_changeHideSome));
    Q_TEST(connect(Tasty::instance()->settings(), &Settings::hideNegativeRatedChanged, this, &FeedModel::_changeHideSome));

    Q_TEST(connect(Tasty::instance(), &Tasty::authorizedChanged, this, &FeedModel::_resetOrReloadRatings));
    Q_TEST(connect(Tasty::instance(), &Tasty::entryCreated,      this, &FeedModel::_prependEntry));
    Q_TEST(connect(Tasty::instance(), &Tasty::entryDeleted,      this, &FeedModel::_removeEntry));
}



FeedModel::~FeedModel()
{
    _clear();
}



int FeedModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _entries.size();
}



QVariant FeedModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _entries.size())
        return QVariant();

    auto entry = _entries.at(index.row());

    if (role == Qt::UserRole)
        return QVariant::fromValue<Entry*>(entry.data());
    else if (role == Qt::UserRole + 1)
        return isUnrepostable(entry->id());

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
    if (!_hasMore || isLoading() || parent.isValid()
            || (_mode == TlogMode && _tlog->id() <= 0 && _tlog->slug().isEmpty())
            || _mode == InvalidMode)
        return;

    qDebug() << "FeedModel::fetchMore";

    QString url = _url;
    if (_mode == TlogMode || _mode == FavoritesMode)
    {
        if (_tlog->id() > 0)
            url = url.arg(_tlog->id());
        else if (!_tlog->slug().isEmpty())
            url = url.arg(_tlog->slug());
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
    else if (!_tag.isEmpty() && (_mode == TlogMode || _mode == MyTlogMode))
        url += QString("%1&page=%3").arg(splitter).arg(_page++);
    else if (!_prevDate.isEmpty())
        url += QString("%1date=%2").arg(splitter).arg(_prevDate);
    else if (_lastEntry)
        url += QString("%1since_entry_id=%2").arg(splitter).arg(_lastEntry);

    if (_prevDate.isEmpty())
    {
        splitter = url.endsWith(".json") ? "?" : "&";
        int limit = _entries.isEmpty() && _query.isEmpty() && _tag.isEmpty() ? 10 : 20;
        url += QString("%1limit=%2").arg(splitter).arg(limit);
    }

    auto opt = _optionsForFetchMore(_mode == MyTlogMode || _mode == MyFavoritesMode
                                    || _mode == MyPrivateMode || _mode == FriendsMode);
    _loadRequest = new ApiRequest(url, opt);

    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject))));

    _initLoad();
}



void FeedModel::setMode(const FeedModel::Mode mode)
{
    reset(mode);
    if (_mode == TlogMode && _tlog->id() <= 0 && _tlog->slug().isEmpty())
        _hasMore = false;
}



void FeedModel::setTlogId(const int tlog)
{
    if (tlog > 0)
        reset(_mode, tlog);
}



int FeedModel::tlogId() const
{
    return _tlog->id();
}



void FeedModel::setSlug(const QString& slug)
{
    if (!slug.isEmpty())
        reset(_mode, -1, slug);
}



QString FeedModel::slug() const
{
    return _tlog->slug();
}



void FeedModel::setMinRating(const int rating)
{
    if (rating > 0 && rating != _minRating)
    {
        _minRating = rating;
        emit minRatingChanged();

        reset();
    }
}



void FeedModel::setQuery(const QString& query)
{
    reset(InvalidMode, 0, QString(), query);
}



void FeedModel::setTag(const QString& tag)
{
    reset(InvalidMode, 0, QString(), QString(), tag);
}



void FeedModel::reset(Mode mode, int tlog, const QString& slug, const QString& query, const QString& tag)
{
    beginResetModel();

    if (tlog > 0)
    {
        _tlog->setId(tlog);
        emit tlogIdChanged();
    }

    if (!slug.isEmpty())
    {
        _tlog->setSlug(slug);
        emit slugChanged();
    }

    _page = 1;
    _query = QUrl::toPercentEncoding(query);
    emit queryChanged();

    _tag = QUrl::toPercentEncoding(tag);
    emit tagChanged();

    if (mode != InvalidMode && mode != _mode)
    {
        _mode = mode;
        emit modeChanged();
    }

    if (_mode == FriendsMode)
        Tasty::instance()->clearUnreadFriendsEntries();

    _setUrl(_mode);

    _errorString.clear();
    emit errorStringChanged();

    _prevDate.clear();

    _hasMore = true;
    _lastEntry = 0;
    delete _loadRequest;

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



bool FeedModel::showFixed() const
{
    return (_mode == LiveMode || _mode == BestMode || _mode == ExcellentMode
            || _mode == GoodMode || _mode == WellMode || _mode == BetterThanMode
            || _mode == FriendsMode || _mode == AnonymousMode);
}



bool FeedModel::isRepostable(int entryId) const
{
    if (_idEntries.contains(entryId))
        return false;

    auto entry = pTasty->dataCache()->entry(entryId);
    if (!entry)
        return false;

    if (entry->tlog()->id() == tlogId())
        return false;

    if (!pTasty->me())
        return false;

    if (_mode == MyTlogMode
            && pTasty->me()->id() == entry->tlog()->id())
        return false;

    if (_mode != TlogMode)
        return false;

    if (!_tlog->flow() || !_tlog->flow()->isWritable())
        return false;

    if (pTasty->me()->id() == tlogId())
        return false;

    return true;
}



bool FeedModel::isUnrepostable(int entryId) const
{
    if (!_idEntries.contains(entryId))
        return false;

    auto entry = pTasty->dataCache()->entry(entryId);
    if (!entry)
        return false;

    if (entry->tlog()->id() == tlogId())
        return false;

    if (!pTasty->me())
        return false;

    if (_mode == MyTlogMode
            && pTasty->me()->id() == entry->tlog()->id())
        return false;

    if (_mode != TlogMode)
        return false;

    if (!_tlog->flow() || !_tlog->flow()->isWritable())
        return false;

    if (pTasty->me()->id() == tlogId())
        return false;

    return true;
}



void FeedModel::setSinceEntryId(int id)
{
    if (id > 0)
        _lastEntry = id + 1;
}



void FeedModel::setSinceDate(const QString& date)
{
    _prevDate = date;
}



void FeedModel::repost(int entryId)
{
    if (_repostRequest)
        return;

    int tlog = _mode == MyTlogMode ? (pTasty->me() ? pTasty->me()->id() : 0) : tlogId();
    auto url = QString("v1/reposts.json");

    _repostRequest = new ApiRequest(url, ApiRequest::AllOptions);
    _repostRequest->addFormData("tlog_id", tlog);
    _repostRequest->addFormData("entry_id", entryId);
    _repostRequest->post();

    Q_TEST(connect(_repostRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addRepost(QJsonObject))));
}



void FeedModel::unrepost(int entryId)
{
    if (_repostRequest || !isUnrepostable(entryId))
        return;

    int tlog = _mode == MyTlogMode ? (pTasty->me() ? pTasty->me()->id() : 0) : tlogId();
    auto url = QString("v1/reposts.json?tlog_id=%1&entry_id=%2").arg(tlog).arg(entryId);
    _repostRequest = new ApiRequest(url, ApiRequest::AllOptions);
    _repostRequest->deleteResource();

    Q_TEST(connect(_repostRequest, SIGNAL(success(QJsonObject)), this, SLOT(_removeRepost(QJsonObject))));
}



QHash<int, QByteArray> FeedModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole]   = "entry";
    roles[Qt::UserRole+1] = "isUnrepostable";
    return roles;
}



void FeedModel::_addItems(const QJsonObject& data)
{
    qDebug() << "FeedModel::_addItems";

    auto feed =  data.contains("items") ? data.value("items").toArray()
                                        : data.value("entries").toArray();

    auto more = (data.contains("prev_date")
                 && !data.value("prev_date").isNull())
            || (data.contains("has_more")
                ? data.value("has_more").toBool()
                : (data.contains("next_since_entry_id")
                   && !data.value("next_since_entry_id").isNull()));

    if (more != _hasMore)
    {
        _hasMore = more;
        emit hasMoreChanged();
    }

    QList<EntryPtr> all;
    QList<EntryPtr> fixed;
    all.reserve(feed.size());
    auto fixMode = showFixed();
    foreach(auto item, feed)
    {
        auto obj = item.toObject();
        auto json = obj.contains("entry") ? obj.value("entry").toObject()
                                          : obj;
        auto id = json.value("id").toInt();
        auto entry = pTasty->dataCache()->entry(id);
        if (!entry)
            entry = EntryPtr::create(nullptr);

        entry->init(json);

        if (fixMode && entry->isFixed())
            fixed << entry;
        else
            all << entry;
    }

    std::sort(fixed.begin(), fixed.end(),
              [](const EntryPtr& left, const EntryPtr& right)
    {
        return left->fixedAt() > right->fixedAt();
    });

    // in live modes next_since_entry_id may be incorrect
    if (fixed.isEmpty() && data.contains("next_since_entry_id"))
        _lastEntry = data.value("next_since_entry_id").toInt();
    else if (_prevDate.isEmpty() && !all.isEmpty())
        _lastEntry = all.last()->entryId();

    if (data.contains("prev_date"))
    {
        auto prev = data.value("prev_date");
        if (prev.isNull())
            _prevDate.clear();
        else
            _prevDate = prev.toString();
    }

    if (all.isEmpty())
    {
        _loadRequest = nullptr;

        if (_hasMore)
            fetchMore(QModelIndex());
        else
            emit loadingChanged();

        return;
    }

    bool loadMore = false;
    if (hideShort() || hideNegative())
    {
        _addSome(fixed, _fixedCount, _allFixedCount);
        int fc = -1, afc = -1;
        loadMore = _addSome(all, fc, afc);
    }
    else
    {
        _addAll(fixed, _allFixedCount);
        int afc = -1;
        _addAll(all, afc);
    }

    _loadRequest = nullptr;

    if (loadMore)
        fetchMore(QModelIndex());
    else
        emit loadingChanged();
}



void FeedModel::_changeHideSome()
{
    beginResetModel();

    if (_allEntries.isEmpty())
        _allEntries = _entries;

    _entries.clear();

    _fixedCount = 0;

    bool s = hideShort();
    bool n = hideNegative();

    for (int i = 0; i < _allEntries.size(); i++)
    {
        const auto& e = _allEntries.at(i);
        if ((!s || e->wordCount() >= 100)
            && (!n || e->rating()->bayesRating() >= 0))
        {
            _entries << e;

            if (i < _allFixedCount)
                _fixedCount++;
        }
    }

    endResetModel();
}



void FeedModel::_resetOrReloadRatings()
{
    if (_mode == MyTlogMode
            || _mode == MyFavoritesMode
            || _mode == MyPrivateMode
            || _mode == FriendsMode)
    {
        if (Tasty::instance()->isAuthorized())
            reset();
        else
            setMode(LiveMode);
    }
    else
        _reloadRatings();
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
    for (auto entry: entries)
        url += QString("%1,").arg(entry->id());
    url.remove(url.size() - 1, 1);

    auto request = new ApiRequest(url);
    request->get();

    Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_setRatings(QJsonArray))));
}



void FeedModel::_setRatings(const QJsonArray& data)
{
    if (_idEntries.isEmpty())
        return;

    foreach (auto rating, data)
    {
        auto id = rating.toObject().value("entry_id").toInt();
        auto entry = _idEntries.value(id);
        Q_ASSERT(entry);
        if (!entry)
            continue;

        entry->rating()->init(rating.toObject());
    }
}



void FeedModel::_addRepost(const QJsonObject& data)
{
    if (data.value("status").toString() == "success")
        emit pTasty->info("Репост добавлен"); //! \todo add entry to feed
    else
    {
        emit pTasty->error(0, "При добавлении репоста произошла ошибка");
        qDebug() << data;
    }
}



void FeedModel::_removeRepost(const QJsonObject& data)
{
    if (data.value("status").toString() == "success")
        emit pTasty->info("Репост удален"); //! \todo remove entry from feed
    else
    {
        emit pTasty->error(0, "При удалении репоста произошла ошибка");
        qDebug() << data;
    }
}



void FeedModel::_prependEntry(int id, int tlogId)
{
    if (!(tlogId == 0 && (_mode == MyTlogMode //! \todo || _mode == FriendsMode && not is private
                          || (_mode == TlogMode && pTasty->me()
                              && _tlog->id() == pTasty->me()->id())))
            && !(tlogId == -1 && _mode == AnonymousMode))
        return;

    auto entry = pTasty->dataCache()->entry(id);
    if (!entry)
        return;

    beginInsertRows(QModelIndex(), 0, 0);

    _entries.insert(_fixedCount, entry);

    if (!_allEntries.isEmpty())
        _allEntries.insert(_allFixedCount, entry);

    _idEntries.insert(id, entry);

    endInsertRows();
}



void FeedModel::_removeEntry(int id)
{
    for (int i = 0; i < _entries.size(); i++)
        if (_entries.at(i)->id() == id)
        {
            beginRemoveRows(QModelIndex(), i, i);
            auto entry = _entries.takeAt(i);
            _allEntries.removeOne(entry);
            _idEntries.remove(id);
            endRemoveRows();

            break;
        }
}



void FeedModel::_addAll(QList<EntryPtr>& all, int& from)
{
    if (from < 0)
        from = _entries.size();

    beginInsertRows(QModelIndex(), from, from + all.size() - 1);

    foreach (auto e, all)
    {
        _entries.insert(from++, e);
        _idEntries.insert(e->id(), e);
    }

    endInsertRows();
}



bool FeedModel::_addSome(QList<EntryPtr>& all, int& from, int& allFrom)
{
    if (allFrom < 0)
        allFrom = _allEntries.size();

    bool s = hideShort();
    bool n = hideNegative();

    QList<EntryPtr> some;
    foreach (auto e, all)
    {
        _allEntries.insert(allFrom++, e);

        if ((!s || e->wordCount() >= 100)
            && (!n || e->rating()->bayesRating() >= 0))
            some << e;
    }

    if (some.isEmpty())
        return true;

    _addAll(some, from);

    return false;
}



void FeedModel::_clear()
{
    _entries.clear();
    _allEntries.clear();
    _idEntries.clear();

    _fixedCount = 0;
    _allFixedCount = 0;
}



void FeedModel::_setUrl(FeedModel::Mode mode)
{
    switch(mode)
    {
    case MyTlogMode:
        if (_tag.isEmpty())
            _url = QString("v1/tlog/%1/entries/tlogs.json")
                    .arg(Tasty::instance()->settings()->userId());
        else
            _url = QString("v1/tlog/%1/entries/tags_tlogs/%2.json")
                    .arg(Tasty::instance()->settings()->userId()).arg(_tag);
        break;
    case MyFavoritesMode:
        _url = QString("v1/tlog/%1/favorites/tlogs.json")
                .arg(Tasty::instance()->settings()->userId());
        break;
    case MyPrivateMode:
        _url = QString("v1/tlog/%1/privates/tlogs.json")
                .arg(Tasty::instance()->settings()->userId());
        break;
    case FriendsMode:
        _url = "v1/my_feeds/friends/tlogs.json";
        break;
    case LiveMode:
        _url = "v1/feeds/live/tlogs.json";
        break;
    case FlowsMode:
        _url = "v1/feeds/live_flows/tlogs.json";
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
        if (_tag.isEmpty())
            _url = QString("v1/tlog/%1/entries/tlogs.json");
        else
            _url = QString("v1/tlog/%2/entries/tags_tlogs/%1.json").arg(_tag);
        break;
    case FavoritesMode:
        _url = "v1/tlog/%1/favorites/tlogs.json";
        break;
    default:
        qDebug() << "feed mode =" << mode;
    }
}
