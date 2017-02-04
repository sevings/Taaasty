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

#include "pusher.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebSocket>
#include <QTimer>
#include <QDebug>

#ifdef QT_DEBUG
#include <QTime>
#endif

#include "channel.h"



Pusher::Pusher(const QString& appKey, QObject *parent)
    : QObject(parent)
    , _socket(new QWebSocket)
    , _imReconnect(false)
    , _pingTimer(new QTimer(this))
    , _pongTimer(new QTimer(this))
    , _reconnectTimer(new QTimer(this))
{
    // TODO: SSL
    _url = QStringLiteral("wss://ws.pusherapp.com:443/app/%1?client=qpusher&version=1.0&protocol=7").arg(appKey);

    _pingTimer->setInterval(120000);
    _pingTimer->setSingleShot(false);

    _pongTimer->setInterval(30000);
    _pongTimer->setSingleShot(true);

    _reconnectTimer->setInterval(500);
    _reconnectTimer->setSingleShot(true);

//    QObject::connect(_socket, &QWebSocket::disconnected,        this, &Pusher::connect);
    QObject::connect(_socket, &QWebSocket::disconnected,        this, &Pusher::_emitDisconnected);
    QObject::connect(_socket, &QWebSocket::textMessageReceived, this, &Pusher::_handleEvent);

    QObject::connect(_socket, &QWebSocket::pong,                _pongTimer, &QTimer::stop);
    QObject::connect(_socket, &QWebSocket::disconnected,        _pingTimer, &QTimer::stop);

    QObject::connect(_pingTimer, SIGNAL(timeout()),             _socket,    SLOT(ping()));
    QObject::connect(_pingTimer, SIGNAL(timeout()),             _pongTimer, SLOT(start()));
    QObject::connect(_pongTimer, SIGNAL(timeout()),             _socket,    SLOT(close()));

    QObject::connect(_reconnectTimer, &QTimer::timeout,         this, &Pusher::connect);

#ifdef QT_DEBUG
    QObject::connect(_socket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),
        [](QAbstractSocket::SocketError error)
    {
        qDebug() << "Socket error:" << error;
    });

    QObject::connect(_pingTimer, &QTimer::timeout, []()
    {
        qDebug() << "pinging pusher at" << QTime::currentTime().toString();
    });

    QObject::connect(_socket, &QWebSocket::pong, []()
    {
        qDebug() << "pong from pusher at" << QTime::currentTime().toString();
    });

    QObject::connect(_pongTimer, &QTimer::timeout, []()
    {
        qDebug() << "pusher has not ponged before" << QTime::currentTime().toString();
    });

    QObject::connect(_socket, &QWebSocket::sslErrors,
                     [](const QList<QSslError>& errors)
    {
        foreach (auto error, errors)
            qDebug() << "Socket SSL error:" << error;
    });
#endif

    connect();
}



Pusher::~Pusher()
{
    delete _socket;
}



bool Pusher::isConnected() const
{
    return _socket->state() == QAbstractSocket::ConnectedState;
}



QString Pusher::socketId() const
{
    return _socketId;
}



Channel* Pusher::channel(const QString& name) const
{
    return _channels.value(name);
}



Channel* Pusher::subscribe(const QString& channelName, bool isPublic)
{
    Channel* ch;
    if (_channels.contains(channelName))
    {
        ch = _channels.value(channelName);
    }
    else
    {
        ch = new Channel(channelName, isPublic, this);
        _channels[channelName] = ch;
    }

    ch->resubscribe();

    return ch;
}



void Pusher::unsubscribe(const QString& channelName)
{
    auto ch = _channels.take(channelName);
    if (ch)
    {
        ch->unsubscribe();
        ch->deleteLater();
    }
}



void Pusher::connect()
{
    if (_socket->state() != QAbstractSocket::UnconnectedState
            && _socket->state() != QAbstractSocket::ClosingState)
        return;

    qDebug() << "Connecting";

    _socket->close();
    _socket->open(_url);
}



void Pusher::disconnect()
{
    qDebug() << "Disconnecting";

    _socket->close();

    qDeleteAll(_channels);
    _channels.clear();
}



void Pusher::reconnect()
{
    if (!isConnected())
    {
        connect();
        return;
    }

    qDebug() << "Reconnecting";

    _imReconnect = true;

    _socket->close();
}



void Pusher::_handleEvent(const QString& message)
{
    _pingTimer->start();

    QJsonParseError jsonError;
    auto doc = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "Pusher json error:" << jsonError.errorString();
        qDebug() << "Json:" << message;
        return;
    }

    auto json = doc.object();
    auto event = json.value(QStringLiteral("event")).toString();
    auto channel = json.value(QStringLiteral("channel")).toString();
    auto data = json.value(QStringLiteral("data")).toString();

    auto ch = _channels.value(channel);

    auto isPusher         = event.startsWith(QStringLiteral("pusher:"));
    auto isPusherInternal = event.startsWith(QStringLiteral("pusher_internal:"));

    if (!isPusher && !isPusherInternal)
    {
        if (ch)
            emit ch->event(event, data);
        return;
    }

    doc = QJsonDocument::fromJson(data.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError)
    {
        qDebug() << "Pusher json error:" << jsonError.errorString();
        qDebug() << "Json:" << data;
        return;
    }
    json = doc.object();

    if (event == "pusher:connection_established")
    {
        _socketId = json.value(QStringLiteral("socket_id")).toString();

#ifdef QT_DEBUG
        qDebug() << "connected to pusher at" << QTime::currentTime().toString();
        qDebug() << "socket id:" << _socketId;
#endif

        auto activityTimeout = json.value(QStringLiteral("activity_timeout")).toInt();
        if (activityTimeout > 0)
        {
            auto interval = qMin(activityTimeout * 1000, _pingTimer->interval());
            _pingTimer->setInterval(interval);
            _pingTimer->start();
        }

        _reconnectTimer->setInterval(500);

        foreach (auto ch, _channels)
            ch->resubscribe();

        emit connected();
        return;
    }

    if (event == "pusher_internal:subscription_succeeded")
    {
        if (!ch)
            return;

        qDebug() << "Subscribed to" << channel;

        auto ids = json.value(QStringLiteral("presence")).toObject().value(QStringLiteral("ids")).toArray();
        foreach (auto id, ids)
            ch->_presenceMemberIds << id.toString();

        ch->_subscribed = true;
        emit ch->subscribed();

        return;
    }

    if (event == "pusher_internal:member_added")
    {
        if (!ch)
            return;

        auto id = json.value(QStringLiteral("user_id")).toString();
        ch->_presenceMemberIds << id;
        emit ch->memberAdded(id);

        return;
    }

    if (event == "pusher_internal:member_removed")
    {
        if (!ch)
            return;

        auto id = json.value(QStringLiteral("user_id")).toString();
        ch->_presenceMemberIds.remove(id);
        emit ch->memberRemoved(id);

        return;
    }

    if (event == "pusher:ping")
    {
        _send("", "pusher:pong", "");
        return;
    }

    if (event == "pusher:error")
    {
        auto code = json.value(QStringLiteral("code")).toInt();
        auto message = json.value(QStringLiteral("message")).toString();

        qDebug() << "Pusher error:" << code << message;

        emit errorEvent(code, message);

//        if (code >= 4000 && code < 4100)
        if (code >= 4100 && code < 4200)
        {
            _reconnectTimer->setInterval(_reconnectTimer->interval() * 2);
            _reconnectTimer->start();

            qDebug() << "Next attempt in" << _reconnectTimer->interval() << "ms";
        }
        else if (code >= 4200 && code < 4300)
            _imReconnect = true;

        return;
    }

    qDebug() << "Unsupported Pusher event:" << event;
    qDebug() << "Data:" << data;
}



void Pusher::_emitDisconnected()
{
    foreach (auto ch, _channels)
        ch->_clear();
    
    auto code = _socket->closeCode();
    auto reason = _socket->closeReason();

    qDebug() << "Disconnected:" << code << reason;

    emit disconnected(code, reason); //-V2006
    
    if (_imReconnect)
        connect();
    
    _imReconnect = false;
}



bool Pusher::_send(const QString& channel, const QString& event, const QString& data)
{
    QJsonObject json;
    json["event"] = event;
    json["data"] = data;
    if (!channel.isEmpty())
        json["channel"] = channel;

    return _send(json);
}



bool Pusher::_send(const QJsonObject& json)
{
    if (!isConnected())
        return false;

    auto data = QJsonDocument(json).toJson(QJsonDocument::Compact);
    _socket->sendTextMessage(data);

    return true;
}
