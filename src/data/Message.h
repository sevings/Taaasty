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

#include "MessageBase.h"

class Conversation;
class AttachedImagesModel;



class Message: public MessageBase
{
    Q_OBJECT

    friend class PusherClient;

    Q_PROPERTY(int      userId          MEMBER _userId         NOTIFY updated)
    Q_PROPERTY(int      recipientId     MEMBER _recipientId    NOTIFY updated)
    Q_PROPERTY(int      conversationId  MEMBER _conversationId NOTIFY updated)
    Q_PROPERTY(User*    replyTo         READ replyTo           NOTIFY updated)

    Q_PROPERTY(AttachedImagesModel* attachedImagesModel MEMBER _attachedImagesModel NOTIFY updated)

public:
    enum MessageType
    {
        UnknownMessageType,
        NormalMessage,
        SystemMessage
    };

    Q_ENUMS(MessageType)

    Message(QObject* parent = nullptr);
    Message(const QJsonObject& data, Conversation* chat, QObject* parent = nullptr);
    ~Message();

    int   userId() const;
    User* replyTo();

signals:
    void updated();

public slots:
    void read();

private slots:
    void _init(const QJsonObject& data);
    void _correctHtml();
    void _markRead(const QJsonObject& data);
    void _updateUser();

private:
    void _updateRead(const QJsonObject& data);
    void _markRemoved(const QJsonObject& data);
    void _setType(const QJsonObject& data);

    int         _userId;
    int         _recipientId;
    int         _conversationId;
    User*       _replyTo; //-V122
    int         _replyUserId;
    MessageType _type;

    Conversation* _chat; //-V122

    AttachedImagesModel* _attachedImagesModel; //-V122
};
