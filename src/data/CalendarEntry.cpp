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

#include <QDate>

#include "Entry.h"

#include "../tasty.h"
#include "../tastydatacache.h"



CalendarEntry::CalendarEntry(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _isPrivate(false)
    , _commentsCount(0)
    , _isFlow(false)
    , _base(nullptr)
{

}



CalendarEntry::CalendarEntry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entry(nullptr)
    , _base(nullptr)
{
    _id              = data.value("entry_id").toInt();
    auto d = data.value("created_at").toString();
    _createdAt       = Tasty::parseDate(d);

    auto date = QDate::fromString(d.left(7), "yyyy-MM");
    auto month = QDate::longMonthName(date.month(), QDate::StandaloneFormat);
    _month           = QString("%1, %2").arg(date.year()).arg(month);

    _date            = d.left(10);
    _url             = data.value("entry_url").toString();
    _type            = data.value("type_symbol").toString();
    _isPrivate       = data.value("is_private").toBool();
    _commentsCount   = data.value("comments_count").toInt();
    _truncatedTitle  = data.value("title_truncated").toString();
    _isFlow          = data.value("is_flow").toBool();
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

    _entry = EntryPtr::create((QObject*)nullptr);
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



int CalendarEntry::id() const
{
    return _id;
}



QString CalendarEntry::month() const
{
    return _month;
}

