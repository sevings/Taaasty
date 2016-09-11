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

#ifndef TASTY_H
#define TASTY_H

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>

#include "settings.h"

class ApiRequest;
class PusherClient;
class User;



class Tasty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isAuthorized        READ isAuthorized        NOTIFY authorized)
    Q_PROPERTY(int  unreadChats         READ unreadChats         NOTIFY unreadChatsChanged)
    Q_PROPERTY(int  unreadNotifications READ unreadNotifications NOTIFY unreadNotificationsChanged)

public:
    explicit Tasty(QNetworkAccessManager* web = nullptr);
    ~Tasty();

    static Tasty* instance(QNetworkAccessManager* web = nullptr);

    Settings*               settings() const { return _settings; }
    QNetworkAccessManager*  manager() const { return _manager; }
    PusherClient*           pusher() const { return _pusher; }

    Q_INVOKABLE bool isAuthorized() const;

    Q_INVOKABLE int unreadChats()         const { return _unreadChats; }
    Q_INVOKABLE int unreadNotifications() const { return _unreadNotifications; }

    Q_INVOKABLE static QString num2str(const int n, const QString str1,
                    const QString str234, const QString str5);
    static QString parseDate(const QString d, const bool bigLetter = true);
    static void correctHtml(QString& html, bool isEntry = true);
    static QString truncateHtml(QString html, int length = 300);

    Q_INVOKABLE void setImageWidth(int entry, int comment);

    User* me();

signals:
    void authorizationNeeded();
    void authorized();

    void unreadChatsChanged();
    void unreadNotificationsChanged();

    void error(const int code, const QString text);
    void info(const QString text);

    void htmlRecorrectionNeeded();

    void networkAccessible();

public slots:
    void authorize(const QString login, const QString password);
    void reconnectToPusher();

private slots:
    void _readAccessToken(const QJsonObject data);

    void _showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible);

    void _setUnreadChats(int count);
    void _setUnreadNotifications(int count);

    void _saveOrReconnect(Qt::ApplicationState state);

private:
    Settings* _settings;
    QNetworkAccessManager* _manager;
    PusherClient* _pusher;

    int _entryImageWidth;
    int _commentImageWidth;

    int _unreadChats;
    int _unreadNotifications;

    User* _me;
};

#endif // TASTY_H
