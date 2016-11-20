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

#include "pusherclient.h"

#include "tasty.h"
#include "apirequest.h"

#include "qpusher/pusher.h"
#include "qpusher/channel.h"

#include "data/Message.h"
#include "data/Comment.h"
#include "data/Notification.h"

#include "defines.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>



PusherClient::PusherClient(Tasty* tasty)
    : QObject(nullptr)
    , _pusher(new Pusher("40dbf1d864d4b366b5e6", this))
    , _tasty(tasty)
{
    if (!tasty)
        return;

    _readyTimer.setInterval(300000);
    _readyTimer.setSingleShot(false);

    Q_TEST(QObject::connect(&_readyTimer, SIGNAL(timeout()), this, SLOT(_sendReady())));

    _addPrivateChannels();

    Q_TEST(QObject::connect(tasty, &Tasty::authorizedChanged, this, &PusherClient::_resubscribe));
    Q_TEST(QObject::connect(tasty, &Tasty::networkAccessible, this, &PusherClient::connect));
}



void PusherClient::addChat(ChatPtr chat)
{
    if (!chat)
        return;

    Q_ASSERT(!chat->parent());

    _chats.insert(chat->id(), chat.toWeakRef());

    if (chat->entryId())
        _chatsByEntry.insert(chat->entryId(), chat.toWeakRef());

    if (chat->type() == Conversation::PrivateConversation)
        _chatsByTlog.insert(chat->recipientId(), chat.toWeakRef());
}



void PusherClient::removeChat(Conversation* chat)
{
    if (!chat)
        return;

    _chats.remove(chat->id());
    _chatsByEntry.remove(chat->entryId());
}



ChatPtr PusherClient::chat(int id) const
{
    return _chats.value(id).toStrongRef();
}



ChatPtr PusherClient::chatByEntry(int entryId) const
{
    return _chatsByEntry.value(entryId).toStrongRef();
}



ChatPtr PusherClient::chatByTlog(int tlogId) const
{
    return _chatsByTlog.value(tlogId).toStrongRef();
}



void PusherClient::addEntry(EntryPtr entry)
{
    Q_ASSERT(!entry || !entry->parent());

    if (entry)
        _entries.insert(entry->entryId(), entry.toWeakRef());
}



void PusherClient::removeEntry(int id)
{
    _entries.remove(id);
}



EntryPtr PusherClient::entry(int id) const
{
    return _entries.value(id).toStrongRef();
}



void PusherClient::addMessage(Message* msg)
{
    _messages.insert(msg->id(), msg);
}



void PusherClient::removeMessage(int id)
{
    _messages.remove(id);
}



Message*PusherClient::message(int id) const
{
    return _messages.value(id);
}



void PusherClient::addComment(Comment* cmt)
{
    _comments.insert(cmt->id(), cmt);
}



void PusherClient::removeComment(int id)
{
    _comments.remove(id);
}



void PusherClient::addNotification(Notification* notif)
{
    _notifications.insert(notif->id(), notif);
}



void PusherClient::removeNotification(int id)
{
    _notifications.remove(id);
}



void PusherClient::connect()
{
    _pusher->connect();
}



void PusherClient::reconnect()
{
    _pusher->reconnect();
}

    

void PusherClient::_resubscribe()
{
    _pusher->unsubscribe(_messagingChannel);
    _pusher->unsubscribe(_friendsChannel);
    _addPrivateChannels();
}



void PusherClient::_sendReady()
{
    if (_pusher->isConnected())
    {
        auto data = QString("socket_id=%1").arg(_pusher->socketId());
        new ApiRequest("v2/messenger/only_ready.json", ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                       QNetworkAccessManager::PostOperation, data);
    }
    else
        _pusher->connect();
}



void PusherClient::_getMessagingAuth()
{
    auto request = _getPusherAuth(_messagingChannel);
    
    Q_TEST(QObject::connect(request, SIGNAL(success(QJsonObject)), 
        this, SLOT(_subscribeToMessaging(QJsonObject))));
}



void PusherClient::_getFriendsAuth()
{
    auto request = _getPusherAuth(_friendsChannel);
    
    Q_TEST(QObject::connect(request, SIGNAL(success(QJsonObject)), 
        this, SLOT(_subscribeToFriends(QJsonObject))));   
}



void PusherClient::_subscribeToMessaging(const QJsonObject data)
{
    auto auth = data.value("auth").toString();
    _pusher->channel(_messagingChannel)->subscribeToPrivate(auth);
}



void PusherClient::_subscribeToFriends(const QJsonObject data)
{
    auto auth = data.value("auth").toString();
    _pusher->channel(_friendsChannel)->subscribeToPrivate(auth);
}



void PusherClient::_handleMessagingEvent(const QString event, const QString data)
{
    qDebug() << "Messaging event:" << event;

    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data.toUtf8(), &jpe).object();
    if (jpe.error != QJsonParseError::NoError)
    {
        qDebug() << "parse error: " << jpe.errorString();
        qDebug() << "json:" << data;
        return;
    }

    if (event.endsWith("status"))
    {
        auto chats = json.value("unreadConversationsCount").toInt();
        emit unreadChats(chats);

        auto notifs = json.value("unreadNotificationsCount").toInt();
        emit unreadNotifications(notifs);

//        auto active = json.value("activeConversationsCount").toInt();

        return;
    }

    if (event == "push_notification")
    {
        emit notification(json);
        return;
    }

    if (event == "typed")
    {
        auto chatId = json.value("conversation_id").toInt();
        auto userId = json.value("user_id").toInt();
        auto chat = _chats.value(chatId);
        if (chat)
            chat.data()->addTyped(userId);
        return;
    }

    if (event.endsWith("update_conversation"))
    {
        auto chatId = json.value("id").toInt();
        auto chat = _chats.value(chatId);
        if (chat)
            chat.data()->init(json);
        return;
    }

    if (event.endsWith("push_message"))
    {
        auto chatId = json.value("conversation_id").toInt();
        auto chat = _chats.value(chatId);
        if (chat)
            emit chat.data()->messageReceived(json);
        else
            emit unreadChat();
        return;
    }

    if (event.endsWith("update_messages"))
    {
        auto chatId = json.value("conversation_id").toInt();
        if (!_chats.contains(chatId))
            return;

        auto messages = json.value("messages").toArray();
        foreach (auto msgData, messages)
        {
            auto msgId = msgData.toObject().value("id").toInt();
            auto msg = _messages.value(msgId);
            if (msg)
                msg->_updateRead(msgData.toObject());
        }

        return;
    }

    if (event.endsWith("delete_user_messages"))
    {
        auto chatId = json.value("conversation_id").toInt();
        if (!_chats.contains(chatId))
            return;

        auto messages = json.value("messages").toArray();
        foreach (auto msgData, messages)
        {
            auto msgId = msgData.toObject().value("id").toInt();
            auto msg = _messages.value(msgId);
            if (msg)
                msg->_markRemoved(msgData.toObject());
        }

        return;
    }

    if (event == "update_notifications")
    {
        auto notifs = json.value("notifications").toArray();
        foreach (auto notifData, notifs)
        {
            auto notifId = notifData.toObject().value("id").toInt();
            if (_notifications.contains(notifId))
                _notifications.value(notifId)->_updateRead(notifData.toObject());
        }

        return;
    }

    qDebug() << "Data:" << data;
}



void PusherClient::_handleFriendsEvent(const QString event, const QString data)
{
    qDebug() << "Friends event:" << event;

    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data.toUtf8(), &jpe).object();
    if (jpe.error != QJsonParseError::NoError)
    {
        qDebug() << "parse error: " << jpe.errorString();
        qDebug() << "json:" << data;
        return;
    }
    
    if (event == "new_entry")
    {
        auto entry = json.value("entry").toObject();
        auto id = entry.value("id").toInt();
//        auto type = entry.value("type").toString(); // TextEntry
        emit unreadFriendsEntry(id);
        return;
    }

    qDebug() << "Data:" << data;
}



void PusherClient::_addPrivateChannels()
{    
    if (!_tasty->isAuthorized())
        return;

    _messagingChannel = QString("private-%1-messaging").arg(_tasty->settings()->userId());
    auto mc = _pusher->subscribe(_messagingChannel, false);

    Q_TEST(QObject::connect(mc, SIGNAL(authNeeded()),           this, SLOT(_getMessagingAuth())));
    Q_TEST(QObject::connect(mc, SIGNAL(subscribed()),           this, SLOT(_sendReady())));
    Q_TEST(QObject::connect(mc, SIGNAL(subscribed()),   &_readyTimer, SLOT(start())));
    Q_TEST(QObject::connect(mc, SIGNAL(event(QString,QString)), this, SLOT(_handleMessagingEvent(QString,QString))));
    
    _friendsChannel = QString("private-%1-friends").arg(_tasty->settings()->userId());
    auto fc = _pusher->subscribe(_friendsChannel, false);

    Q_TEST(QObject::connect(fc, SIGNAL(authNeeded()),           this, SLOT(_getFriendsAuth())));
    Q_TEST(QObject::connect(fc, SIGNAL(event(QString,QString)), this, SLOT(_handleFriendsEvent(QString,QString))));
}



ApiRequest* PusherClient::_getPusherAuth(const QString channel)
{
    auto data = QString("socket_id=%1&channel_name=%2").arg(_pusher->socketId()).arg(channel);
    return new ApiRequest("v2/messenger/auth.json",
                          ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                          QNetworkAccessManager::PostOperation, data);
}
