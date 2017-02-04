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

#include "Notification.h"

#include "User.h"
#include "Entry.h"
#include "Author.h"

#include "../tasty.h"
#include "../tastydatacache.h"
#include "../apirequest.h"



Notification::Notification(QObject* parent)
    : TastyData(parent)
    , _sender(new User(this))
    , _read(false)
    , _entityId(0)
    , _entityType(UnknownType)
    , _parentId(0)
    , _entityUser(nullptr)
{

}



Notification::Notification(const QJsonObject& data, QObject *parent)
    : TastyData(parent)
    , _entityUser(nullptr)
{
    _id         = data.value(QStringLiteral("id")).toInt();
    _createdAt  = Tasty::parseDate(data.value(QStringLiteral("created_at")).toString());
    _sender     = new User(data.value(QStringLiteral("sender")).toObject(), this);
    _read       = !data.value(QStringLiteral("read_at")).isNull();
    _action     = data.value(QStringLiteral("action")).toString();
    _actionText = data.value(QStringLiteral("action_text")).toString();
    _text       = data.value(QStringLiteral("text")).toString();
    _entityId   = data.value(QStringLiteral("entity_id")).toInt();

    auto entityType = data.value(QStringLiteral("entity_type")).toString();
    if (entityType == "Entry")
    {
        _entityType = EntryType;
        
        if (data.contains(QStringLiteral("entity")))
        {
            _entry = EntryPtr::create(nullptr);
            _entry->init(data.value(QStringLiteral("entity")).toObject());
        }
    }
    else if (entityType == "Relationship")
    {
        _entityType = RelationshipType;
        
        if (data.contains(QStringLiteral("entity")))
        {
            auto userData = data.value(QStringLiteral("entity")).toObject().value(QStringLiteral("user")).toObject();
            _entityUser = new Author(userData, this);
        }
    }
    else if (entityType == "Comment")
        _entityType = CommentType;
    else
    {
        qDebug() << "Unknown notification entity type:" << entityType;
        _entityType = UnknownType;
    }

    _parentId   = data.value(QStringLiteral("parent_id")).toInt();
    _parentType = data.value(QStringLiteral("parent_type")).toString();

    pTasty->dataCache()->addNotification(this);

    emit idChanged();
}



Notification::~Notification()
{
    pTasty->dataCache()->removeNotification(_id);
}



int Notification::entityId() const
{
    return _entityId;
}



User* Notification::sender() const
{
    return _sender;
}



QString Notification::actionText() const
{
    return _actionText;
}



QString Notification::text() const
{
    return _text;
}



bool Notification::isRead() const
{
    return _read;
}



int Notification::parentId() const
{
    return _parentId;
}



Entry* Notification::entry()
{
    if (_entry)
        return _entry.data();

    int id = 0;
    switch (_entityType)
    {
    case EntryType:
        id = _entityId;
        break;
    case CommentType:
        id = _parentId;
        break;
    default:
        break;
    }

    if (id <= 0)
        return nullptr;

    _entry = pTasty->dataCache()->entry(id);
    if (_entry)
        return _entry.data();

    _entry = EntryPtr::create(nullptr);
    _entry->setId(id);
    return _entry.data();
}



Author* Notification::entityUser() const
{
    return _entityUser;
}



void Notification::read()
{
    if (_read || _id <= 0 || isLoading())
        return;

    auto url = QStringLiteral("v2/messenger/notifications/%1/read.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _request->put();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_updateRead(QJsonObject))));
    
    _initRequest();
}



void Notification::_updateRead(const QJsonObject& data)
{
    if (_read || data.value(QStringLiteral("id")).toInt() != _id)
        return;

    _read = !data.value(QStringLiteral("read_at")).isNull();
    emit readChanged();
}
