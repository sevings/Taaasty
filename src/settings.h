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

    Q_PROPERTY(QString      login               READ login                  WRITE setLogin                  NOTIFY loginChanged)
    Q_PROPERTY(QString      accessToken         READ accessToken            WRITE setAccessToken            NOTIFY accessTokenChanged)
    Q_PROPERTY(int          userId              READ userId                 WRITE setUserId                 NOTIFY userIdChanged)

    Q_PROPERTY(QString      prevLogin           READ prevLogin              WRITE setPrevLogin              NOTIFY prevLoginChanged)
    Q_PROPERTY(QString      prevAccessToken     READ prevAccessToken        WRITE setPrevAccessToken        NOTIFY prevAccessTokenChanged)
    Q_PROPERTY(int          prevUserId          READ prevUserId             WRITE setPrevUserId             NOTIFY prevUserIdChanged)

    Q_PROPERTY(int          maxImageWidth       READ maxImageWidth          WRITE setMaxImageWidth          NOTIFY maxImageWidthChanged)
    Q_PROPERTY(bool         autoloadImages      READ autoloadImages         WRITE setAutoloadImages         NOTIFY autoloadImagesChanged)
    Q_PROPERTY(bool         loadImagesOverWifi  READ loadImagesOverWifi     WRITE setLoadImagesOverWifi     NOTIFY loadImagesOverWifiChanged)
    Q_PROPERTY(int          maxLoadImageSize    READ maxLoadImageSize       WRITE setMaxLoadImageSize       NOTIFY maxLoadImageSizeChanged)
    Q_PROPERTY(int          maxCacheSize        READ maxCacheSize           WRITE setMaxCacheSize           NOTIFY maxCacheSizeChanged)

    Q_PROPERTY(bool         hideShortPosts      READ hideShortPosts         WRITE setHideShortPosts         NOTIFY hideShortPostsChanged)
    Q_PROPERTY(bool         hideNegativeRated   READ hideNegativeRated      WRITE setHideNegativeRated      NOTIFY hideNegativeRatedChanged)
    Q_PROPERTY(bool         darkTheme           READ darkTheme              WRITE setDarkTheme              NOTIFY darkThemeChanged)
    Q_PROPERTY(quint8       fontZoom            READ fontZoom               WRITE setFontZoom               NOTIFY fontZoomChanged)
    Q_PROPERTY(bool         systemNotifications READ systemNotifications    WRITE setSystemNotifications    NOTIFY systemNotificationsChanged)
    Q_PROPERTY(bool         predictiveText      READ predictiveText         WRITE setPredictiveText         NOTIFY predictiveTextChanged)

    Q_PROPERTY(QString      lastTitle           READ lastTitle              WRITE setLastTitle              NOTIFY lastTitleChanged)
    Q_PROPERTY(QString      lastText            READ lastText               WRITE setLastText               NOTIFY lastTextChanged)
    Q_PROPERTY(QString      lastSource          READ lastSource             WRITE setLastSource             NOTIFY lastSourceChanged)
    Q_PROPERTY(QStringList  lastImages          READ lastImages             WRITE setLastImages             NOTIFY lastImagesChanged)
    Q_PROPERTY(bool         lastOptimizeImages  READ lastOptimizeImages     WRITE setLastOptimizeImages     NOTIFY lastOptimizeImagesChanged)
    Q_PROPERTY(int          lastPrivacy         READ lastPrivacy            WRITE setLastPrivacy            NOTIFY lastPrivacyChanged)
    Q_PROPERTY(int          lastPostingTlog     READ lastPostingTlog        WRITE setLastPostingTlog        NOTIFY lastPostingTlogChanged)
    Q_PROPERTY(quint8       lastEntryType       READ lastEntryType          WRITE setLastEntryType          NOTIFY lastEntryTypeChanged)

public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    void swapProfiles();
    void clearProfile();

    QString login() const;
    void setLogin(const QString& login);

    QString accessToken() const;
    void setAccessToken(const QString& token);

    QDateTime expiresAt() const;
    void setExpiresAt(const QDateTime& date);
    void setExpiresAt(const QString& date);

    int userId() const;
    void setUserId(const int id);

    bool saveProfile() const;
    void setSaveProfile(const bool save);

    QString prevLogin() const;
    void setPrevLogin(const QString& login);

    QString prevAccessToken() const;
    void setPrevAccessToken(const QString& token);

    QDateTime prevExpiresAt() const;
    void setPrevExpiresAt(const QDateTime& date);
    void setPrevExpiresAt(const QString& date);

    int prevUserId() const;
    void setPrevUserId(const int id);

    bool prevSaveProfile() const;
    void setPrevSaveProfile(const bool save);

    int maxImageWidth() const;
    void setMaxImageWidth(const int width);

    bool autoloadImages() const;
    void setAutoloadImages(bool load);

    bool loadImagesOverWifi() const;
    void setLoadImagesOverWifi(bool load);

    int maxLoadImageSize() const;
    void setMaxLoadImageSize(int size);

    int maxCacheSize() const;
    void setMaxCacheSize(int size);

    quint8 cacheVersion() const;
    void setCacheVersion(quint8 v);

    bool hideShortPosts() const;
    void setHideShortPosts(bool hide);

    bool hideNegativeRated() const;
    void setHideNegativeRated(bool hide);

    bool darkTheme() const;
    void setDarkTheme(bool dark);

    quint8 fontZoom() const;
    void setFontZoom(quint8 zoom);

    bool systemNotifications() const;
    void setSystemNotifications(bool enable);

    bool predictiveText() const;
    void setPredictiveText(bool enable);

    QString lastTitle() const;
    void setLastTitle(const QString& title);

    QString lastText() const;
    void setLastText(const QString& text);

    QString lastSource() const;
    void setLastSource(const QString& source);

    QStringList lastImages() const;
    void setLastImages(const QStringList& images);

    bool lastOptimizeImages() const;
    void setLastOptimizeImages(bool opt);

    int lastPrivacy() const;
    void setLastPrivacy(int privacy);

    int lastPostingTlog() const;
    void setLastPostingTlog(int tlog);

    quint8 lastEntryType() const;
    void setLastEntryType(quint8 type);

signals:
    void loginChanged();
    void accessTokenChanged();
    void userIdChanged();

    void prevLoginChanged();
    void prevAccessTokenChanged();
    void prevUserIdChanged();

    void maxImageWidthChanged();
    void autoloadImagesChanged(bool);
    void loadImagesOverWifiChanged(bool);
    void maxLoadImageSizeChanged(int);
    void maxCacheSizeChanged(int);

    void hideShortPostsChanged();
    void hideNegativeRatedChanged();
    void darkThemeChanged();
    void fontZoomChanged();
    void systemNotificationsChanged();
    void predictiveTextChanged();

    void lastTitleChanged();
    void lastTextChanged();
    void lastSourceChanged();
    void lastImagesChanged();
    void lastOptimizeImagesChanged();
    void lastPrivacyChanged();
    void lastPostingTlogChanged();
    void lastEntryTypeChanged();

public slots:

private:
    QSettings _settings;
};

#endif // SETTINGS_H
