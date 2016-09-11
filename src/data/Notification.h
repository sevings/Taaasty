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
#include "TastyData.h"

class User;



class Notification: public TastyData
{
    Q_OBJECT

    friend class PusherClient;
    friend class NotificationsModel;
    
    Q_PROPERTY(QString      createdAt  MEMBER _createdAt  CONSTANT)
    Q_PROPERTY(User*        sender     MEMBER _sender     CONSTANT)
    Q_PROPERTY(bool         isRead     MEMBER _read       NOTIFY readChanged)
    Q_PROPERTY(QString      action     MEMBER _action     CONSTANT)
    Q_PROPERTY(QString      actionText MEMBER _actionText CONSTANT)
    Q_PROPERTY(QString      text       MEMBER _text       CONSTANT) //! \todo show updates
    Q_PROPERTY(int          entityId   MEMBER _entityId   CONSTANT)
    Q_PROPERTY(EntityType   entityType MEMBER _entityType CONSTANT)
    Q_PROPERTY(int          parentId   MEMBER _parentId   CONSTANT)
    Q_PROPERTY(QString      parentType MEMBER _parentType CONSTANT)
    
    Q_PROPERTY(Entry*       entry      READ entry         CONSTANT)

public:
    enum EntityType
    {
        UnknownType         = 0,
        EntryType           = 1,
        CommentType         = 2,
        RelationshipType    = 3
    };

    Q_ENUMS(EntityType)

    Notification(QObject* parent = nullptr);
    Notification(const QJsonObject data, QObject* parent = nullptr);
    ~Notification();
    
    int entityId() const;

    User* sender() const;

    QString actionText() const;

    QString text() const;

    bool isRead() const;

    int parentId() const;

    Entry* entry();

public slots:
    void read();

signals:
    void readChanged();

private slots:
    void _updateRead(const QJsonObject data);

private:
    QString     _createdAt;
    User*       _sender;
    bool        _read;
    QString     _action;
    QString     _actionText;
    QString     _text;
    int         _entityId;
    EntityType  _entityType;
    int         _parentId;
    QString     _parentType;
    //QString _image;

    EntryPtr    _entry;
};
