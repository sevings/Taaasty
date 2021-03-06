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

#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>

#include "tastylistmodel.h"

class CalendarEntry;
class Entry;
class ApiRequest;



class CalendarModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(SortOrder sortOrder READ sortOrder WRITE sort NOTIFY sortOrderChanged)
    Q_PROPERTY(bool loadingRatings READ loadingRatings       NOTIFY loadingRatingsChanged)

public:
    enum SortOrder
    {
        NewestFirst,
        OldestFirst,
        BestFirst
    };

    Q_ENUMS(SortOrder)

    CalendarModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;
    virtual QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setTlog(int tlog);

    int lastEntryId() const;

    CalendarEntry* at(int row) const;

    Q_INVOKABLE CalendarEntry* firstMonthEntry(QString month) const;

    SortOrder sortOrder() const;
    void sort(SortOrder order);

    bool loadingRatings() const;

signals:
    void loaded();
    void sortOrderChanged();
    void loadingRatingsChanged();

public slots:
    virtual void loadMore() override;

private slots:
    void _setCalendar(const QJsonObject& data);
    void _setRatings(const QJsonArray& data);

private:
    void _loadRatings();
    void _sort();

    QList<CalendarEntry*>           _calendar;
    QHash<int, CalendarEntry*>      _idEntries;
    QHash<QString, CalendarEntry*>  _firstMonthEntries;
    int                             _tlog;
    SortOrder                       _order;
    int                             _ratingsLoaded;
    QList<ApiRequestPtr>            _ratingRequests;
};

#endif // CALENDARMODEL_H
