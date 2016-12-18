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

#ifndef PUSHER_H
#define PUSHER_H

#include <QObject>
#include <QHash>
#include <QUrl>

class Channel;
class QWebSocket;
class QTimer;



class Pusher : public QObject
{
    Q_OBJECT

    friend class Channel;

public:
    Pusher(const QString& appKey, QObject* parent = nullptr);
    ~Pusher();

    bool isConnected() const;
    QString socketId() const;

    Channel* channel(const QString& name) const;

    Channel* subscribe(const QString& channelName, bool isPublic = true);
    void   unsubscribe(const QString& channelName);

signals:
    void connected();
    void disconnected(int code, const QString& reason);

    void errorEvent(const int code, const QString& message);

public slots:
    void connect();
    void disconnect();
    void reconnect();

private slots:
    void _handleEvent(const QString& message);
    void _emitDisconnected();

private:
    bool _send(const QString& channel, const QString& event, const QString& data);
    bool _send(const QJsonObject& json);

    QUrl        _url;
    QString     _socketId;
    QWebSocket* _socket; //-V122
    bool        _imReconnect;

    QHash<QString, Channel*> _channels;

    QTimer* _pingTimer;      //-V122
    QTimer* _pongTimer;      //-V122
    QTimer* _reconnectTimer; //-V122
};

#endif // PUSHER_H
