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
#include <QHash>

#include "tastylistmodel.h"

class CalendarEntry;
class Entry;
class ApiRequest;



class CalendarModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(bool isPrivate   READ isPrivate  NOTIFY isPrivateChanged)
    
public:
    CalendarModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setTlog(const int tlog);

    int lastEntryId() const;

    CalendarEntry* at(int row) const;

    Q_INVOKABLE CalendarEntry* firstMonthEntry(QString month) const;

    bool isPrivate() const { return _isPrivate; }
    
signals:
    void isPrivateChanged();
    
    void loaded();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _setCalendar(QJsonObject data);
    void _setPrivate(int errorCode);

private:
    QList<CalendarEntry*>           _calendar;
    QHash<QString, CalendarEntry*>  _firstMonthEntries;
    bool                            _isPrivate;
};

#endif // CALENDARMODEL_H
