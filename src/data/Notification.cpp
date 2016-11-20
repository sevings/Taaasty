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

#include "../tasty.h"
#include "../tastydatacache.h"
#include "../apirequest.h"



Notification::Notification(QObject* parent)
    : TastyData(parent)
    , _sender(new User(this))
    , _read(false)
    , _entityId(0)
    , _parentId(0)
{

}



Notification::Notification(const QJsonObject data, QObject *parent)
    : TastyData(parent)
{
    _id         = data.value("id").toInt();
    _createdAt  = Tasty::parseDate(data.value("created_at").toString());
    _sender     = new User(data.value("sender").toObject(), this);
    _read       = !data.value("read_at").isNull();
    _action     = data.value("action").toString();
    _actionText = data.value("action_text").toString();
    _text       = data.value("text").toString();
    _entityId   = data.value("entity_id").toInt();

    auto entityType = data.value("entity_type").toString();
    if (entityType == "Entry")
        _entityType = EntryType;
    else if (entityType == "Relationship")
        _entityType = RelationshipType;
    else if (entityType == "Comment")
        _entityType = CommentType;
    else
    {
        qDebug() << "Unknown notification entity type:" << entityType;
        _entityType = UnknownType;
    }

    _parentId   = data.value("parent_id").toInt();
    _parentType = data.value("parent_type").toString();

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
    if (_entry )
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

    _entry = EntryPtr::create((QObject*)nullptr);
    _entry->setId(id);
    return _entry.data();
}



void Notification::read()
{
    if (_read || _id <= 0 || isLoading())
        return;

    auto url = QString("v2/messenger/notifications/%1/read.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PutOperation);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_updateRead(QJsonObject))));
    
    _initRequest();
}



void Notification::_updateRead(const QJsonObject data)
{
    if (_read || data.value("id").toInt() != _id)
        return;

    _read = !data.value("read_at").isNull();
    emit readChanged();
}
