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

#include "channel.h"

#include <QJsonObject>
#include <QDebug>

#include "pusher.h"



void Channel::subscribeToPrivate(const QString& auth, const QString& data)
{
    if (_subscribed)
        return;

    _sendSubscription(auth, data);
}



bool Channel::isSubscribed() const
{
    return _subscribed;
}



QString Channel::name() const
{
    return _name;
}



QSet<QString> Channel::presenceMemberIds() const
{
    return _presenceMemberIds;
}



void Channel::resubscribe()
{
    if (_subscribed)
        return;

    if (!_pusher->isConnected())
        return;

    qDebug() << "Subscribing to" << _name;

    if (_isPublic)
        _sendSubscription();
    else
        QMetaObject::invokeMethod(this, "authNeeded", Qt::QueuedConnection);
}



void Channel::unsubscribe()
{
    if (!_subscribed)
        return;

    if (!_sendUnsubscription())
        return;

    _clear();

    _pusher->unsubscribe(_name);
}



Channel::Channel(const QString& name, bool isPublic, Pusher* pusher)
    : QObject(pusher)
    , _subscribed(false)
    , _isPublic(isPublic)
    , _name(name)
    , _pusher(pusher)
{
    Q_ASSERT(pusher);
}



bool Channel::_sendSubscription(const QString& auth, const QString& channelData)
{
    QJsonObject json;
    json["event"] = "pusher:subscribe";

    QJsonObject data;
    data["channel"] = _name;

    if (!auth.isEmpty())
        data["auth"] = auth;

    if (!channelData.isEmpty())
        data["channel_data"] = channelData;

    json["data"] = data;

    return _pusher->_send(json);
}



bool Channel::_sendUnsubscription()
{
    QJsonObject json;
    json["event"] = "pusher:unsubscribe";

    QJsonObject data;
    data["channel"] = _name;

    json["data"] = data;

    return _pusher->_send(json);
}



void Channel::_clear()
{
    _presenceMemberIds.clear();
    _subscribed = false;
    emit unsubscribed();
}
