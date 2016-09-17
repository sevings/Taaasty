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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDateTime>



class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  login               READ login                  WRITE setLogin                  NOTIFY loginChanged)
    Q_PROPERTY(QString  accessToken         READ accessToken            WRITE setAccessToken            NOTIFY accessTokenChanged)
    Q_PROPERTY(int      userId              READ userId                 WRITE setUserId                 NOTIFY userIdChanged)

    Q_PROPERTY(QString  prevLogin           READ prevLogin              WRITE setPrevLogin              NOTIFY prevLoginChanged)
    Q_PROPERTY(QString  prevAccessToken     READ prevAccessToken        WRITE setPrevAccessToken        NOTIFY prevAccessTokenChanged)
    Q_PROPERTY(int      prevUserId          READ prevUserId             WRITE setPrevUserId             NOTIFY prevUserIdChanged)

    Q_PROPERTY(int      maxImageWidth       READ maxImageWidth          WRITE setMaxImageWidth          NOTIFY maxImageWidthChanged)
    Q_PROPERTY(bool     autoloadImages      READ autoloadImages         WRITE setAutoloadImages         NOTIFY autoloadImagesChanged)
    Q_PROPERTY(bool     hideShortPosts      READ hideShortPosts         WRITE setHideShortPosts         NOTIFY hideShortPostsChanged)
    Q_PROPERTY(bool     hideNegativeRated   READ hideNegativeRated      WRITE setHideNegativeRated      NOTIFY hideNegativeRatedChanged)
    Q_PROPERTY(bool     darkTheme           READ darkTheme              WRITE setDarkTheme              NOTIFY darkThemeChanged)
    Q_PROPERTY(bool     systemNotifications READ systemNotifications    WRITE setSystemNotifications    NOTIFY systemNotificationsChanged)
    Q_PROPERTY(QString  lastTitle           READ lastTitle              WRITE setLastTitle              NOTIFY lastTitleChanged)
    Q_PROPERTY(QString  lastText            READ lastText               WRITE setLastText               NOTIFY lastTextChanged)

public:
    explicit Settings(QObject *parent = 0);

    void swapProfiles();

    QString login() const;
    void setLogin(const QString login);

    QString accessToken() const;
    void setAccessToken(const QString token);

    QDateTime expiresAt() const;
    void setExpiresAt(const QDateTime date);
    void setExpiresAt(const QString date);

    int userId() const;
    void setUserId(const int id);

    bool saveProfile() const;
    void setSaveProfile(const bool save);

    QString prevLogin() const;
    void setPrevLogin(const QString login);

    QString prevAccessToken() const;
    void setPrevAccessToken(const QString token);

    QDateTime prevExpiresAt() const;
    void setPrevExpiresAt(const QDateTime date);
    void setPrevExpiresAt(const QString date);

    int prevUserId() const;
    void setPrevUserId(const int id);

    bool prevSaveProfile() const;
    void setPrevSaveProfile(const bool save);

    int maxImageWidth() const;
    void setMaxImageWidth(const int width);

    bool autoloadImages() const;
    void setAutoloadImages(bool load);

    bool hideShortPosts() const;
    void setHideShortPosts(bool hide);

    bool hideNegativeRated() const;
    void setHideNegativeRated(bool hide);

    bool darkTheme() const;
    void setDarkTheme(bool dark);

    bool systemNotifications() const;
    void setSystemNotifications(bool enable);

    QString lastTitle() const;
    void setLastTitle(const QString title);

    QString lastText() const;
    void setLastText(const QString text);

signals:
    void loginChanged();
    void accessTokenChanged();
    void userIdChanged();

    void prevLoginChanged();
    void prevAccessTokenChanged();
    void prevUserIdChanged();

    void maxImageWidthChanged();
    void autoloadImagesChanged(bool);
    void hideShortPostsChanged();
    void hideNegativeRatedChanged();
    void darkThemeChanged();
    void systemNotificationsChanged();
    void lastTitleChanged();
    void lastTextChanged();

public slots:

private:
    QSettings _settings;

};

#endif // SETTINGS_H
