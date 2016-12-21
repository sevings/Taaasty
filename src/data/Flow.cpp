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

#include "Flow.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"



Flow::Flow(QObject *parent)
    : TastyData(parent)
    , _isPrivate(false)
    , _isPremoderate(false)
    , _isEditable(false)
    , _isWritable(false)
{

}



void Flow::setId(const int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    _request = new ApiRequest(QString("v1/flows/%1.json").arg(_id));
    _request->get();
    
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



bool Flow::isWritable() const
{
    return _isWritable;
}



bool Flow::isEditable() const
{
    return _isEditable;
}



void Flow::init(const QJsonObject& data)
{
    _id             = data.value("id").toInt();
    _name           = data.value("name").toString();
    _slug           = data.value("slug").toString();
    _title          = data.value("title").toString();
    _url            = data.value("tlog_url").toString();
    _pic            = data.value("flowpic").toObject().value("original_url").toString();
    _isPrivate      = data.value("is_privacy").toBool();
    _isPremoderate  = data.value("is_premoderate").toBool();
    _isEditable     = data.value("can_edit").toBool();
    _isWritable     = data.value("can_write").toBool();
    _followersCount = Tasty::num2str(data.value("followers_count").toInt(),
                                     "подписчик", "подписчика", "подписчиков");
    _entriesCount   = Tasty::num2str(data.value("public_tlog_entries_count").toInt(),
                                     "запись", "записи", "записей");

    emit idChanged();
    emit updated();
}
