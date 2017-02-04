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

#include "calendarmodel.h"

#include <QJsonArray>
#include <QDebug>

#include "../data/Rating.h"
#include "../data/CalendarEntry.h"
#include "../apirequest.h"



CalendarModel::CalendarModel(QObject* parent)
    : TastyListModel(parent)
    , _tlog(0)
    , _order(NewestFirst)
    , _ratingsLoaded(0)
{
    qDebug() << "CalendarModel";
}



int CalendarModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _calendar.size();
}



QVariant CalendarModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _calendar.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<CalendarEntry*>(_calendar.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool CalendarModel::canFetchMore(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _tlog > 0 && _calendar.isEmpty() && !isLoading();
}



void CalendarModel::fetchMore(const QModelIndex& parent)
{
    if (isLoading() || !canFetchMore(parent))
        return;

    QString url = QStringLiteral("v1/tlog/%1/calendar.json").arg(_tlog);
    _loadRequest = new ApiRequest(url);

    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_setCalendar(QJsonObject))));

    _initLoad();
}



QHash<int, QByteArray> CalendarModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "entry";
    return roles;
}



void CalendarModel::setTlog(int tlog)
{
    if (tlog <= 0 || _tlog == tlog)
        return;

    _tlog = tlog;

    beginResetModel();

    qDeleteAll(_calendar);
    _calendar.clear();
    _firstMonthEntries.clear();

    endResetModel();

    fetchMore(QModelIndex());
}



int CalendarModel::lastEntryId() const
{
    if (_calendar.isEmpty())
        return 0;

    return _calendar.last()->id();
}



CalendarEntry* CalendarModel::at(int row) const
{
    Q_ASSERT(row >= 0 && row < _calendar.size());

    return _calendar.at(row);
}



CalendarEntry* CalendarModel::firstMonthEntry(QString month) const
{
    auto e = _firstMonthEntries.value(month);
    return e;
}



CalendarModel::SortOrder CalendarModel::sortOrder() const
{
    return _order;
}



void CalendarModel::sort(CalendarModel::SortOrder order)
{
    if (order == _order)
        return;

    _order = order;
    emit sortOrderChanged();

    _sort();
}



bool CalendarModel::loadingRatings() const
{
    for (auto request: _ratingRequests)
        if (request && request->isRunning())
            return true;

    return false;
}



void CalendarModel::loadMore()
{
    if (_calendar.isEmpty())
        fetchMore(QModelIndex());
    else if (_order == BestFirst && !_ratingsLoaded)
        _loadRatings();
}



void CalendarModel::_setCalendar(const QJsonObject& data)
{
    beginResetModel();

    auto periods = data.value(QStringLiteral("periods")).toArray();
    for (int i = periods.size() - 1; i >= 0; i--)
    {
        auto markers = periods.at(i).toObject().value(QStringLiteral("markers")).toArray();
        for (int j = 0; j < markers.size(); j++)
        {
            auto entry = new CalendarEntry(markers.at(j).toObject(), this);
            if (entry->id() <= 0)
            {
                delete entry;
                continue;
            }

            _calendar << entry;
            _idEntries.insert(entry->id(), entry);

            if (!_firstMonthEntries.contains(entry->month()))
                _firstMonthEntries.insert(entry->month(), entry);
        }
    }

    endResetModel();

    if (_order != NewestFirst)
        _sort();

    emit loaded();
}



void CalendarModel::_setRatings(const QJsonArray& data)
{
    for (auto rating: data)
    {
        auto id = rating.toObject().value(QStringLiteral("entry_id")).toInt();
        auto entry = _idEntries.value(id);
        Q_ASSERT(entry);
        if (!entry)
            continue;

        entry->rating()->init(rating.toObject());
    }

    _ratingsLoaded += data.size();
    if (_ratingsLoaded < _calendar.size())
        return;

    _sort();

    emit loadingRatingsChanged();
}



void CalendarModel::_loadRatings()
{
    QString url(QStringLiteral("v1/ratings.json?ids="));
    url.reserve(200 * 9 + 20);

    auto addRequest = [&]()
    {
        url.remove(url.size() - 1, 1);

        auto request = new ApiRequest(url, ApiRequest::ShowMessageOnError);
        request->get();

        Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_setRatings(QJsonArray))));

        _ratingRequests << request;

        url = "v1/ratings.json?ids=";
    };

    int i = 0;
    for (auto entry: _calendar)
    {
        url += QStringLiteral("%1,").arg(entry->id());

        if (++i % 200 == 0)
            addRequest();
    }

    if (i % 200 > 0)
        addRequest();

    emit loadingRatingsChanged();
}



void CalendarModel::_sort()
{
    if (_order == BestFirst && _ratingsLoaded < _calendar.size())
    {
        _loadRatings();
        return;
    }

    beginResetModel();

#define SORT_CALENDAR \
    std::sort(_calendar.begin(), _calendar.end(), \
              [](const CalendarEntry* left, const CalendarEntry* right)

    switch (_order)
    {
        case NewestFirst:
            SORT_CALENDAR
            {
                return left->dateTime() > right->dateTime();
            });
            break;
        case OldestFirst:
            SORT_CALENDAR
            {
                return left->dateTime() < right->dateTime();
            });
            break;
        case BestFirst:
            SORT_CALENDAR
            {
                auto diff = left->rating()->votes() - right->rating()->votes();
                if (diff)
                    return diff > 0;
                return left->dateTime() > right->dateTime();
            });
            break;
        default:
            qDebug() << "Unknown calendar sorting order:" << _order;
    }

    _firstMonthEntries.clear();
    for (auto entry: _calendar)
        if (!_firstMonthEntries.contains(entry->month()))
            _firstMonthEntries.insert(entry->month(), entry);

    endResetModel();
}
