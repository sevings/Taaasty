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

#include "User.h"

#include "../defines.h"

#include "../apirequest.h"



User::User(QObject* parent)
    : TastyData(parent)
{

}



User::User(const QJsonObject data, QObject *parent)
    : TastyData(parent)
{
    _init(data);
}



void User::setId(int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    _request = new ApiRequest(QString("v2/tlog/%1.json").arg(_id));
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_initFromTlog(QJsonObject))));
    
    _initRequest();
}



QString User::name() const
{
    return _name;
}



QString User::slug() const
{
    return _slug;
}



User& User::operator=(const User& other)
{
    if (&other == this)
        return *this;

    _id         = other._id;
    _tlogUrl    = other._tlogUrl;
    _name       = other._name;
    _slug       = other._slug;

    _originalPic     = other._originalPic    ;
    _largePic        = other._largePic       ;
    _thumb128        = other._thumb128       ;
    _thumb64         = other._thumb64        ;
    _symbol          = other._symbol         ;
    _backgroundColor = other._backgroundColor;
    _nameColor       = other._nameColor      ;

    _request = other._request;
    if (_request)
    {
        Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_initFromTlog(QJsonObject))));
    }

    emit idChanged();

    return *this;
}



void User::_init(const QJsonObject data)
{
    _id         = data.value("id").toInt();
    _tlogUrl    = data.value("tlog_url").toString();
    _name       = data.value("name").toString();
    _slug       = data.value("slug").toString();

    auto userpic = data.value("userpic").toObject();

    _originalPic    = userpic.value("original_url").toString();
    if (!_originalPic.isEmpty())
    {
        _largePic   = userpic.contains("large_url")    ? userpic.value("large_url").toString()
                        : QString("http://thumbor4.tasty0.ru/unsafe/800x800/%1").arg(_originalPic);
        _thumb128   = userpic.contains("thumb128_url") ? userpic.value("thumb128_url").toString()
                        : QString("http://thumbor4.tasty0.ru/unsafe/128x128/%1").arg(_originalPic);
        _thumb64    = userpic.contains("thumb64_url")  ? userpic.value("thumb64_url").toString()
                        : QString("http://thumbor4.tasty0.ru/unsafe/64x64/%1").arg(_originalPic);
    }
    _symbol         = userpic.value("symbol").toString();

    auto colors = userpic.value("default_colors").toObject();

    _backgroundColor = colors.value("background").toString();
    _nameColor       = colors.value("name").toString();

    emit idChanged();
    emit updated();
}



void User::_initFromTlog(const QJsonObject data)
{
    auto author = data.value("author").toObject();
    _init(author);
}
