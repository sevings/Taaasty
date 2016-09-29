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

#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QHash>
#include <QTimer>
#include <QWeakPointer>

#include "data/Entry.h"
#include "data/Conversation.h"

class Pusher;
class Tasty;
class Message;
class Comment;
class Notification;



class PusherClient : public QObject
{
    Q_OBJECT
public:
    explicit PusherClient(Tasty* tasty = nullptr);

    void addChat(ChatPtr chat);
    void removeChat(Conversation* chat);

    ChatPtr chat(int id) const;
    ChatPtr chatByEntry(int entryId) const;

    void addEntry(EntryPtr entry);
    void removeEntry(int id);
    EntryPtr entry(int id) const;

    void addMessage(Message* msg);
    void removeMessage(int id);
    Message* message(int id) const;

    void addComment(Comment* cmt);
    void removeComment(int id);

    void addNotification(Notification* notif);
    void removeNotification(int id);

signals:
    void notification(const QJsonObject data);
    void unreadChat();
    void unreadChats(int count);
    void unreadNotifications(int count);
    void unreadFriendsEntry(int entryId);

public slots:
    void connect();
    void reconnect();

private slots:
    void _resubscribe();
    void _sendReady();
    
    void _getMessagingAuth();
    void _getFriendsAuth();
    
    void _subscribeToMessaging(const QJsonObject data);
    void _subscribeToFriends(const QJsonObject data);
        
    void _handleMessagingEvent(const QString event, const QString data);
    void _handleFriendsEvent(const QString event, const QString data);
        
private:
    void        _addPrivateChannels();
    ApiRequest* _getPusherAuth(const QString channel);

    Pusher* _pusher;
    QString _messagingChannel;
    QString _friendsChannel;

    Tasty* _tasty;

    QTimer _readyTimer;

    QHash<int, QWeakPointer<Conversation>>  _chats;
    QHash<int, QWeakPointer<Conversation>>  _chatsByEntry;
    QHash<int, QWeakPointer<Entry>>         _entries;
    QHash<int, Message*>                    _messages;
    QHash<int, Comment*>                    _comments;
    QHash<int, Notification*>               _notifications;
};

#endif // PUSHERCLIENT_H
