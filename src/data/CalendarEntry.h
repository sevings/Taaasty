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

#pragma once

#include <QObject>
#include <QJsonObject>

#include "../defines.h"

class EntryBase;



class CalendarEntry: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int     id             MEMBER _id             CONSTANT)
    Q_PROPERTY(QString createdAt      MEMBER _createdAt      CONSTANT)
    Q_PROPERTY(QString month          MEMBER _month          CONSTANT)
    Q_PROPERTY(QString date           MEMBER _date           CONSTANT)
    Q_PROPERTY(QString url            MEMBER _url            CONSTANT)
    Q_PROPERTY(bool    isPrivate      MEMBER _isPrivate      CONSTANT)
    Q_PROPERTY(QString type           MEMBER _type           CONSTANT)
    Q_PROPERTY(int     commentsCount  MEMBER _commentsCount  CONSTANT)
    Q_PROPERTY(QString truncatedTitle MEMBER _truncatedTitle CONSTANT)
    Q_PROPERTY(bool    isFlow         MEMBER _isFlow         CONSTANT)

    Q_PROPERTY(Entry*  fullEntry      READ fullEntry         CONSTANT)

public:
    CalendarEntry(QObject* parent = nullptr);
    CalendarEntry(const QJsonObject data, QObject* parent = nullptr);

    Entry* fullEntry();

    EntryPtr full();
    EntryBase* base();

    int     id() const;
    QString month() const;

private:
    int     _id;
    QString _createdAt;
    QString _month;
    QString _date;
    QString _url;
    bool    _isPrivate;
    QString _type;
    int     _commentsCount;
    QString _truncatedTitle;
    bool    _isFlow;

    EntryPtr   _entry;
    EntryBase* _base;
};
