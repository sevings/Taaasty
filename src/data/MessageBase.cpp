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

#include "MessageBase.h"

#include "../tasty.h"



MessageBase::MessageBase(QObject* parent)
    : TastyData(parent)
    , _user(nullptr)
    , _read(true)
    , _containsImage(false)
{

}



bool MessageBase::isRead() const
{
    return _read;
}



QDateTime MessageBase::createdDate() const
{
    return _date;
}



void MessageBase::_setTruncatedText()
{
    _truncatedText = Tasty::truncateHtml(_text);    
    _containsImage = _text.contains("<img", Qt::CaseInsensitive);
}



void MessageBase::_setDate(const QString d)
{
    _date = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
}



User* MessageBase::user() const
{
    return _user;
}



QString MessageBase::text() const
{
    return _text;
}
