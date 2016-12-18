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

#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include <QSet>

class Pusher;



class Channel : public QObject
{
    Q_OBJECT

    friend class Pusher;

public:
    void subscribeToPrivate(const QString& auth, const QString& data = QString());

    bool isSubscribed() const;

    QString name() const;

    QSet<QString> presenceMemberIds() const;

public slots:
    void resubscribe();
    void unsubscribe();

signals:
    void authNeeded();

    void subscribed();
    void unsubscribed();

    void event(const QString& event, const QString& data);

    void memberAdded(const QString& id);
    void memberRemoved(const QString& id);

private:
    Channel(const QString& name, bool isPublic, Pusher* pusher);

    bool _sendSubscription(const QString& auth = QString(),
                           const QString& channelData = QString());

    bool _sendUnsubscription();

    void _clear();

    bool _subscribed;
    bool _isPublic;

    QString _name;

    QSet<QString> _presenceMemberIds;

    Pusher* _pusher; //-V122
};

#endif // CHANNEL_H
