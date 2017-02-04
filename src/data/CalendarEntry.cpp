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

#include "CalendarEntry.h"

#include <QQmlEngine>

#include "Entry.h"
#include "Rating.h"

#include "../tasty.h"
#include "../tastydatacache.h"



CalendarEntry::CalendarEntry(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _isPrivate(false)
    , _commentsCount(0)
    , _isFlow(false)
    , _base(nullptr)
    , _rating(nullptr)
{

}



CalendarEntry::CalendarEntry(const QJsonObject& data, QObject *parent)
    : QObject(parent)
    , _base(nullptr)
    , _rating(new Rating(this))
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    _id              = data.value(QStringLiteral("entry_id")).toInt();
    auto d = data.value(QStringLiteral("created_at")).toString();
    _createdAt       = Tasty::parseDate(d);

    _dateTime = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
    auto date = _dateTime.date();
    auto month = QDate::longMonthName(date.month(), QDate::StandaloneFormat);
    _month           = QStringLiteral("%1, %2").arg(date.year()).arg(month);

    _date            = d.left(10);
    _url             = data.value(QStringLiteral("entry_url")).toString();
    _type            = data.value(QStringLiteral("type_symbol")).toString();
    _isPrivate       = data.value(QStringLiteral("is_private")).toBool();
    _commentsCount   = data.value(QStringLiteral("comments_count")).toInt();
    _truncatedTitle  = data.value(QStringLiteral("title_truncated")).toString();
    _isFlow          = data.value(QStringLiteral("is_flow")).toBool();
}



Entry* CalendarEntry::fullEntry()
{
    return full().data();
}



EntryPtr CalendarEntry::full()
{
    if (_entry)
        return _entry;

    _entry = pTasty->dataCache()->entry(_id);
    if (_entry)
        return _entry;

    _entry = EntryPtr::create(nullptr);
    _entry->setId(_id);
    return _entry;
}



EntryBase* CalendarEntry::base()
{
    if (_base)
        return _base;

    _base = new EntryBase(this);
    _base->load(_id);
    return _base;
}



Rating* CalendarEntry::rating() const
{
    if (_entry)
        return _entry->rating();

    return _rating;
}



int CalendarEntry::id() const
{
    return _id;
}



QString CalendarEntry::month() const
{
    return _month;
}



QString CalendarEntry::date() const
{
    return _date;
}
