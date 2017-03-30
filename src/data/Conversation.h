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
#include <QJsonArray>
#include <QHash>
#include <QEnableSharedFromThis>
#include <QTimer>
#include <QMap>

#include "../defines.h"
#include "TastyData.h"

class MessageBase;
class Message;
class User;
class Author;
class MessagesModel;



class Conversation: public TastyData, public QEnableSharedFromThis<Conversation>
{
    Q_OBJECT

    friend class PusherClient;
    friend class MessagesModel;

    Q_PROPERTY(ConversationType type            MEMBER _type            NOTIFY updated)
    Q_PROPERTY(int              unreadCount     MEMBER _unreadCount     NOTIFY unreadCountChanged)
    Q_PROPERTY(int              unreceivedCount MEMBER _unreceivedCount NOTIFY updated)
    Q_PROPERTY(int              totalCount      MEMBER _totalCount      NOTIFY updated)
    Q_PROPERTY(int              userId          MEMBER _userId          NOTIFY updated)
    Q_PROPERTY(int              recipientId     READ recipientId        NOTIFY updated)
    Q_PROPERTY(bool             isDisabled      MEMBER _isDisabled      NOTIFY updated)
    Q_PROPERTY(bool             notDisturb      MEMBER _notDisturb      NOTIFY updated)
    Q_PROPERTY(bool             canTalk         MEMBER _canTalk         NOTIFY updated)
    Q_PROPERTY(bool             canDelete       MEMBER _canDelete       NOTIFY updated)
    Q_PROPERTY(bool             isAnonymous     MEMBER _isAnonymous     NOTIFY updated)
    Q_PROPERTY(QString          topic           MEMBER _topic           NOTIFY updated)
    Q_PROPERTY(Entry*           entry           READ entry              NOTIFY updated)
//    Q_PROPERTY(QList<User*>     users           MEMBER _users           NOTIFY updated)
//    Q_PROPERTY(QList<User*>     deletedUsers    MEMBER _deletedUsers    NOTIFY updated)
    Q_PROPERTY(Author*          recipient       READ recipient          NOTIFY updated)
    Q_PROPERTY(MessagesModel*   messages        READ messages           CONSTANT)
    Q_PROPERTY(bool  isMyLastMessageUnread   READ isMyLastMessageUnread NOTIFY isMyLastMessageUnreadChanged)
    Q_PROPERTY(MessageBase*     lastMessage     READ lastMessage        NOTIFY lastMessageChanged)
    Q_PROPERTY(bool             isInvolved      READ isInvolved         NOTIFY isInvolvedChanged)
    Q_PROPERTY(QString          typedUsers      READ typedUsers         NOTIFY typedUsersChanged)
    Q_PROPERTY(bool             isTyped         READ isTyped            NOTIFY typedUsersChanged)

public:
    enum ConversationType {
        UninitializedConversation,
        PublicConversation,
        GroupConversation,
        PrivateConversation
    };

    Q_ENUMS(ConversationType)

    Conversation(QObject* parent = nullptr);
    ~Conversation();

    void setId(int id);
    void setRecipientId(int id);
    void setSlug(const QString& slug);
    void setEntryId(int entryId);

    int totalCount() const;

    bool isAnonymous() const;

    MessagesModel* messages() const;

    User* user(int id, bool reloadUsers = true);

    MessageBase* lastMessage() const;

    ConversationType type() const;

    bool isInvolved() const;

    bool isMyLastMessageUnread() const;

    int unreadCount() const;

    Entry* entry();

    int entryId() const;

    int userId() const;
    int recipientId() const;

    Author* recipient() const;

    QString typedUsers();
    bool isTyped() const;
    void addTyped(int userId);
    void removeTyped(int userId);

    void updateUsers();

public slots:
    void init(const QJsonObject& data);

    void update();
    void sendMessage(const QString& text);
    void readTo(int id);
    void readAll();
    void syncRead();
    void leave();
    void remove();

    void sendTyped();

signals:
    void updated();
    void usersUpdated();

    void unreadCountChanged();

    void messageSent(const QJsonObject);
    void messageReceived(const QJsonObject);
    void sendingMessageError();

    void allMessagesRead(const QJsonObject& data);
    void isMyLastMessageUnreadChanged();
    void lastMessageChanged();

    void left(int id);
    void isInvolvedChanged();

    void typedUsersChanged();

private slots:
    void _markRead(const QJsonObject& data);
    void _emitLeft(const QJsonObject& data);
    void _decUnread(bool read);
    void _removeTypedUser();
    void _sendTyped();
    void _initUsers(const QJsonArray& data);
    void _setLastMessage();

private:
    ConversationType    _type;
    int                 _unreadCount;
    int                 _unreceivedCount;
    int                 _totalCount;
    int                 _userId;
    int                 _recipientId;
    bool                _isDisabled;
    bool                _notDisturb;
    bool                _canTalk;
    bool                _canDelete;
    bool                _isAnonymous;
    QString             _topic;
    int                 _entryId;
    QHash<int, User*>   _users;
    QHash<int, User*>   _deletedUsers;
    QHash<int, User*>   _leftUsers;
    Author*             _recipient; //-V122
    MessagesModel*      _messages; //-V122
    QPointer<Message>   _lastMessage;
    QMap<int, QTimer*>  _typedUsers;

    EntryPtr _entry;

    QTimer*              _typedTimer; //-V122
    bool                 _hadTyped;
    QPointer<ApiRequest> _typedRequest;

    QPointer<ApiRequest> _sendRequest;
    QPointer<ApiRequest> _readRequest;
    QPointer<ApiRequest> _usersRequest;
};
