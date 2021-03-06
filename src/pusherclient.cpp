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

#include "pusherclient.h"

#include "tasty.h"
#include "tastydatacache.h"
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



bool PusherClient::isConnected() const
{
    return _pusher->isConnected();
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
        auto request = new ApiRequest("v2/messenger/only_ready.json", ApiRequest::AccessTokenRequired);
        request->addFormData("socket_id", _pusher->socketId());
        request->post();
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



void PusherClient::_subscribeToMessaging(const QJsonObject& data)
{
    auto auth = data.value(QLatin1String("auth")).toString();
    _pusher->channel(_messagingChannel)->subscribeToPrivate(auth);
}



void PusherClient::_subscribeToFriends(const QJsonObject& data)
{
    auto auth = data.value(QLatin1String("auth")).toString();
    _pusher->channel(_friendsChannel)->subscribeToPrivate(auth);
}



void PusherClient::_handleMessagingEvent(const QString& event, const QString& data)
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

    if (event.endsWith(QLatin1String("status")))
    {
        auto chats = json.value(QLatin1String("unreadConversationsCount")).toInt();
        emit unreadChats(chats);

        auto notifs = json.value(QLatin1String("unreadNotificationsCount")).toInt();
        emit unreadNotifications(notifs);

//        auto active = json.value(QLatin1String("activeConversationsCount")).toInt();

        return;
    }

    if (event == "push_notification")
    {
        emit notification(json);
        return;
    }

    if (event == "typed")
    {
        auto chatId = json.value(QLatin1String("conversation_id")).toInt();
        auto userId = json.value(QLatin1String("user_id")).toInt();
        auto chat = pTasty->dataCache()->chat(chatId);
        if (chat)
            chat->addTyped(userId);
        return;
    }

    if (event.endsWith(QLatin1String("update_conversation")))
    {
        auto chatId = json.value(QLatin1String("id")).toInt();
        auto chat = pTasty->dataCache()->chat(chatId);
        if (chat)
            chat->init(json);
        return;
    }

    if (event.endsWith(QLatin1String("push_message")))
    {
        auto chatId = json.value(QLatin1String("conversation_id")).toInt();
        auto chat = pTasty->dataCache()->chat(chatId);
        if (chat)
            emit chat->messageReceived(json);
        else
            emit unreadChat();
        return;
    }

    if (event.endsWith(QLatin1String("update_messages")))
    {
        auto chatId = json.value(QLatin1String("conversation_id")).toInt();
        auto chat = pTasty->dataCache()->chat(chatId);
        if (!chat)
            return;

        auto messages = json.value(QLatin1String("messages")).toArray();
        foreach (auto msgData, messages)
        {
            auto msgId = msgData.toObject().value(QLatin1String("id")).toInt();
            auto msg = pTasty->dataCache()->message(msgId);
            if (msg)
                msg->_updateRead(msgData.toObject());
        }

        return;
    }

    if (event.endsWith(QLatin1String("delete_user_messages")))
    {
        auto chatId = json.value(QLatin1String("conversation_id")).toInt();
        auto chat = pTasty->dataCache()->chat(chatId);
        if (!chat)
            return;

        auto messages = json.value(QLatin1String("messages")).toArray();
        foreach (auto msgData, messages)
        {
            auto msgId = msgData.toObject().value(QLatin1String("id")).toInt();
            auto msg = pTasty->dataCache()->message(msgId);
            if (msg)
                msg->_markRemoved(msgData.toObject());
        }

        return;
    }

    if (event == "update_notifications")
    {
        auto notifs = json.value(QLatin1String("notifications")).toArray();
        foreach (auto notifData, notifs)
        {
            auto notifId = notifData.toObject().value(QLatin1String("id")).toInt();
            auto notif = pTasty->dataCache()->notification(notifId);
            if (notif)
                notif->_updateRead(notifData.toObject());
        }

        return;
    }

    qDebug() << "Data:" << data;
}



void PusherClient::_handleFriendsEvent(const QString& event, const QString& data)
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
        auto entry = json.value(QLatin1String("entry")).toObject();
        auto id = entry.value(QLatin1String("id")).toInt();
//        auto type = entry.value(QLatin1String("type")).toString(); // TextEntry
        emit unreadFriendsEntry(id);
        return;
    }

    qDebug() << "Data:" << data;
}



void PusherClient::_addPrivateChannels()
{    
    if (!_tasty->isAuthorized())
        return;

    _messagingChannel = QStringLiteral("private-%1-messaging").arg(_tasty->settings()->userId());
    auto mc = _pusher->subscribe(_messagingChannel, false);

    Q_TEST(QObject::connect(mc, SIGNAL(authNeeded()),           this, SLOT(_getMessagingAuth())));
    Q_TEST(QObject::connect(mc, SIGNAL(subscribed()),           this, SLOT(_sendReady())));
    Q_TEST(QObject::connect(mc, SIGNAL(subscribed()),   &_readyTimer, SLOT(start())));
    Q_TEST(QObject::connect(mc, SIGNAL(event(QString,QString)), this, SLOT(_handleMessagingEvent(QString,QString))));
    
    _friendsChannel = QStringLiteral("private-%1-friends").arg(_tasty->settings()->userId());
    auto fc = _pusher->subscribe(_friendsChannel, false);

    Q_TEST(QObject::connect(fc, SIGNAL(authNeeded()),           this, SLOT(_getFriendsAuth())));
    Q_TEST(QObject::connect(fc, SIGNAL(event(QString,QString)), this, SLOT(_handleFriendsEvent(QString,QString))));
}



ApiRequest* PusherClient::_getPusherAuth(const QString& channel)
{
    auto request = new ApiRequest("v2/messenger/auth.json", ApiRequest::AccessTokenRequired);
    request->addFormData("socket_id", _pusher->socketId());
    request->addFormData("channel_name", channel);
    request->post();
    return request;
}
