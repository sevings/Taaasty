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
#include <QQmlEngine>

#include "../defines.h"

#include "../tasty.h"
#include "../pusherclient.h"
#include "../settings.h"
#include "../apirequest.h"

#include "../data/Rating.h"



FeedModel::FeedModel(QObject* parent)
    : TastyListModel(parent)
    , _tlog(0)
    , _mode(InvalidMode)
    , _lastEntry(0)
    , _minRating(0)
    , _page(1)
{
    qDebug() << "FeedModel";

    Q_TEST(connect(Tasty::instance()->settings(), &Settings::hideShortPostsChanged,    this, &FeedModel::_changeHideSome));
    Q_TEST(connect(Tasty::instance()->settings(), &Settings::hideNegativeRatedChanged, this, &FeedModel::_changeHideSome));

    Q_TEST(connect(Tasty::instance(), &Tasty::authorized, this, &FeedModel::_resetOrReloadRatings));
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
        return QVariant::fromValue<Entry*>(_entries.at(index.row()).data());

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
            || (_mode == TlogMode && _tlog <= 0 && _slug.isEmpty())
            || _mode == InvalidMode)
        return;

    qDebug() << "FeedModel::fetchMore";

    QString url = _url;
    if (_mode == TlogMode || _mode == FavoritesMode)
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



void FeedModel::setTag(const QString tag)
{
    reset (InvalidMode, 0, QString(), QString(), tag);
}



void FeedModel::reset(Mode mode, int tlog, QString slug, QString query, QString tag)
{
    beginResetModel();

    if (tlog > 0)
        _tlog = tlog;

    if (!slug.isEmpty())
        _slug = slug;

    _page = 1;
    _query = QUrl::toPercentEncoding(query);
    emit queryChanged();

    _tag = QUrl::toPercentEncoding(tag);
    emit tagChanged();

    if (mode != InvalidMode)
        _mode = mode;

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

    auto request = new ApiRequest("v1/entries/text.json", ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addNewPost(QJsonObject))));
}



void FeedModel::postAnonymous(const QString title, const QString content)
{
    auto data = QString("title=%1&text=%2")
            .arg(title)
            .arg(content);

    qDebug() << data;

    auto request = new ApiRequest("v1/entries/anonymous.json", ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_addNewPost(const QJsonObject))));
}



void FeedModel::setSinceEntryId(int id)
{
    if (id > 0)
        _lastEntry = id + 1;
}



void FeedModel::setSinceDate(const QString date)
{
    _prevDate = date;
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
    all.reserve(feed.size());
    foreach(auto item, feed)
    {
        auto obj = item.toObject();
        auto json = obj.contains("entry") ? obj.value("entry").toObject()
                                          : obj;
        auto id = json.value("id").toInt();
        auto entry = Tasty::instance()->pusher()->entry(id);
        if (!entry)
            entry = EntryPtr::create((QObject*)nullptr);

        entry->init(json);

        all << entry;
    }

    if (data.contains("next_since_entry_id"))
    {
        _lastEntry = data.value("next_since_entry_id").toInt();
    }
    else if (_prevDate.isEmpty())
    {
        for (int i = all.size() - 1; i >= 0; i--)
            if (!all.at(i)->isFixed())
            {
                _lastEntry = all.at(i)->entryId();
                break;
            }

        if (_lastEntry <= 0)
            _lastEntry = all.last()->entryId();
    }

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
        loadMore = _addSome(all);
    else
        _addAll(all);

    _loadRequest = nullptr;

    if (loadMore)
        fetchMore(QModelIndex());
    else
        emit loadingChanged();
}



void FeedModel::_addNewPost(QJsonObject data)
{
    auto entry = EntryPtr::create((QObject*)nullptr);
    entry->init(data);

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



void FeedModel::_resetOrReloadRatings()
{
    if (_mode == MyTlogMode
            || _mode == MyFavoritesMode
            || _mode == MyPrivateMode
            || _mode == FriendsMode)
        reset();
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



void FeedModel::_addAll(QList<EntryPtr>& all)
{
    beginInsertRows(QModelIndex(), _entries.size(), _entries.size() + all.size() - 1);
    _entries << all;
    endInsertRows();
}



bool FeedModel::_addSome(QList<EntryPtr>& all)
{
    bool s = hideShort();
    bool n = hideNegative();

    QList<EntryPtr> some;
    foreach (auto e, all)
    {
        _allEntries << e;

        if ((!s || e->wordCount() >= 100)
            && (!n || e->rating()->bayesRating() >= 0))
            some << e;
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
    _entries.clear();
    _allEntries.clear();

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
