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

#include "../defines.h"

#include "../data/CalendarEntry.h"
#include "../apirequest.h"



CalendarModel::CalendarModel(QObject* parent)
    : TastyListModel(parent)
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



void CalendarModel::setTlog(const int tlog)
{
    if (tlog <= 0 || isLoading())
        return;

    QString url = QString("v1/tlog/%1/calendar.json").arg(tlog);
    _loadRequest = new ApiRequest(url);
    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_setCalendar(QJsonObject))));

    _initLoad();

    beginResetModel();

    qDeleteAll(_calendar);
    _calendar.clear();
    _firstMonthEntries.clear();

    endResetModel();
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



QHash<int, QByteArray> CalendarModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "entry";
    return roles;
}



void CalendarModel::_setCalendar(QJsonObject data)
{
    beginResetModel();

    auto periods = data.value("periods").toArray();
    for (int i = periods.size() - 1; i >= 0; i--)
    {
        auto markers = periods.at(i).toObject().value("markers").toArray();
        for (int j = 0; j < markers.size(); j++)
        {
            auto entry = new CalendarEntry(markers.at(j).toObject(), this);
            if (entry->id() <= 0)
            {
                delete entry;
                continue;
            }

            _calendar << entry;

            if (!_firstMonthEntries.contains(entry->month()))
                _firstMonthEntries.insert(entry->month(), entry);
        }
    }

    endResetModel();

    emit loaded();
}
