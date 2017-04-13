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
#include <QQmlApplicationEngine>
#include <QRegularExpression>

#include "defines.h"

#include "settings.h"

#define pTasty Tasty::instance()

class ApiRequest;
class PusherClient;
class User;
class Tlog;
class TastyDataCache;



class Tasty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool  isAuthorized         READ isAuthorized         NOTIFY authorizedChanged)
    Q_PROPERTY(int   unreadChats          READ unreadChats          NOTIFY unreadChatsChanged)
    Q_PROPERTY(int   unreadNotifications  READ unreadNotifications  NOTIFY unreadNotificationsChanged)
    Q_PROPERTY(int   unreadFriendsEntries READ unreadFriendsEntries NOTIFY unreadFriendsEntriesChanged)
    Q_PROPERTY(User* me                   READ me                   NOTIFY authorizedChanged)
    Q_PROPERTY(Tlog* myTlog               READ myTlog               NOTIFY authorizedChanged)

public:
    explicit Tasty();
    ~Tasty();

    static Tasty* instance();

    Settings*               settings()  const { return _settings; }
    QNetworkAccessManager*  manager()   const { return _manager; }
    PusherClient*           pusher()    const { return _pusher; }
    TastyDataCache*         dataCache() const { return _dataCache; }

    bool isAuthorized() const;

    int unreadChats()          const { return _unreadChats; }
    int unreadNotifications()  const { return _unreadNotifications; }
    int unreadFriendsEntries() const { return _unreadFriendsEntries.size(); }

    QList<EntryPtr> clearUnreadFriendsEntries();

    Q_INVOKABLE static QString num2str(const int n, const QString& str1,
                    const QString& str234, const QString& str5);
    static QString parseDate(const QString& d, const bool bigLetter = true);
    static void correctHtml(QString& html, bool isEntry = true);
    static QString truncateHtml(QString html, int length = 300);

    Q_INVOKABLE void setImageWidth(int entry, int comment);

    User* me();
    Tlog* myTlog();

signals:
    void authorizationNeeded();
    void authorizedChanged();

    void unreadChatsChanged();
    void unreadNotificationsChanged();
    void unreadFriendsEntriesChanged();

    void error(const int code, const QString& text);
    void info(const QString& text);

    void htmlRecorrectionNeeded();

    void networkAccessible();
    void networkNotAccessible();

    void entryCreated(int entry, int tlog);
    void entryDeleted(int entry);

    void flowCreated(const QJsonObject& data);

public slots:
    void authorize(const QString& login, const QString& password, bool save);
    void logout();
    void swapProfiles();
    void reconnectToPusher();

private slots:
    void _init();

    void _swapProfiles();
    void _readAccessToken(const QJsonObject& data);

    void _showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible);

    void _setUnreadChats(int count);
    void _setUnreadNotifications(int count);
    void _addUnreadFriendEntry(int entryId);

    void _saveOrReconnect(Qt::ApplicationState state);

private:
    void _finishLogin();

    QQmlApplicationEngine* _engine;     //-V122
    Settings*              _settings;   //-V122
    QNetworkAccessManager* _manager;    //-V122
    PusherClient*          _pusher;     //-V122
    TastyDataCache*        _dataCache;  //-V122

    int _entryImageWidth;
    int _commentImageWidth;

    int _unreadChats;
    int _unreadNotifications;

    QList<EntryPtr> _unreadFriendsEntries;

    Tlog* _myTlog; //-V122

    bool _saveProfile;

    static QRegularExpression _firstSlugRe;
    static QRegularExpression _slugRe;
    static QRegularExpression _imageLinkRe;
    static QRegularExpression _imgRe;
    static QRegularExpression _tagRe;
};

#endif // TASTY_H
