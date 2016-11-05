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

#include "settings.h"

#include <QGuiApplication>
#include <QScreen>
#include <QDebug>



Settings::Settings(QObject *parent)
    : QObject(parent)
    , _settings(this)
{
    qDebug() << "Settings";

    if (!_settings.contains("max_image_width"))
    {
        auto screen = qApp->primaryScreen();
        auto width = screen->size().width();
        setMaxImageWidth(width);
    }

    if (!_settings.contains("autoload_images"))
        setAutoloadImages(true);
}



Settings::~Settings()
{
    if (!saveProfile())
        clearProfile();
}



void Settings::swapProfiles()
{
    auto save  = saveProfile();
    auto login = save ? this->login() : QString();
    auto token = save ? accessToken() : QString();
    auto until = save ? expiresAt()   : QDateTime();
    auto id    = save ? userId()      : 0;

    setSaveProfile(prevSaveProfile());
    setLogin(prevLogin());
    setAccessToken(prevAccessToken());
    setExpiresAt(prevExpiresAt());
    setUserId(prevUserId());

    setPrevSaveProfile(save);
    setPrevLogin(login);
    setPrevAccessToken(token);
    setPrevExpiresAt(until);
    setPrevUserId(id);
}



void Settings::clearProfile()
{
    setSaveProfile(false);
    setLogin(QString());
    setAccessToken(QString());
    setExpiresAt(QDateTime());
    setUserId(0);
}



QString Settings::login() const
{
    return _settings.value("login").toString();
}



void Settings::setLogin(const QString login)
{
    if (login == this->login())
        return;

    _settings.setValue("login", login);

    emit loginChanged();
}



QString Settings::accessToken() const
{
    return _settings.value("access_token").toString();
}



void Settings::setAccessToken(const QString token)
{
    if (token == accessToken())
        return;

    _settings.setValue("access_token", token);

    emit accessTokenChanged();
}



QDateTime Settings::expiresAt() const
{
    return _settings.value("expires_at").toDateTime();
}



void Settings::setExpiresAt(const QDateTime date)
{
    if (date == expiresAt())
        return;

    _settings.setValue("expires_at", date);
}



void Settings::setExpiresAt(const QString date)
{
    auto at = QDateTime::fromString(date.left(19), "yyyy-MM-ddTHH:mm:ss");
    setExpiresAt(at);
}



int Settings::userId() const
{
    return _settings.value("user_id").toInt();
}



void Settings::setUserId(const int id)
{
    if (id == userId())
        return;

    _settings.setValue("user_id", id);

    emit userIdChanged();
}



bool Settings::saveProfile() const
{
    return _settings.value("save_profile", true).toBool();
}



void Settings::setSaveProfile(const bool save)
{
    if (save == saveProfile())
        return;

    _settings.setValue("save_profile", save);
}



QString Settings::prevLogin() const
{
    return _settings.value("prev_login").toString();
}



void Settings::setPrevLogin(const QString login)
{
    if (login == this->prevLogin())
        return;

    _settings.setValue("prev_login", login);

    emit prevLoginChanged();
}



QString Settings::prevAccessToken() const
{
    return _settings.value("prev_access_token").toString();
}



void Settings::setPrevAccessToken(const QString token)
{
    if (token == prevAccessToken())
        return;

    _settings.setValue("prev_access_token", token);

    emit prevAccessTokenChanged();
}



QDateTime Settings::prevExpiresAt() const
{
    return _settings.value("prev_expires_at").toDateTime();
}



void Settings::setPrevExpiresAt(const QDateTime date)
{
    if (date == prevExpiresAt())
        return;

    _settings.setValue("prev_expires_at", date);
}



void Settings::setPrevExpiresAt(const QString date)
{
    auto at = QDateTime::fromString(date.left(19), "yyyy-MM-ddTHH:mm:ss");
    setPrevExpiresAt(at);
}



int Settings::prevUserId() const
{
    return _settings.value("prev_user_id").toInt();
}



void Settings::setPrevUserId(const int id)
{
    if (id == prevUserId())
        return;

    _settings.setValue("prev_user_id", id);

    emit prevUserIdChanged();
}



bool Settings::prevSaveProfile() const
{
    return _settings.value("prev_save_profile", true).toBool();
}



void Settings::setPrevSaveProfile(const bool save)
{
    if (save == prevSaveProfile())
        return;

    _settings.setValue("prev_save_profile", save);
}



int Settings::maxImageWidth() const
{
    return _settings.value("max_image_width").toInt();
}



void Settings::setMaxImageWidth(const int width)
{
    if (width == maxImageWidth())
        return;

    _settings.setValue("max_image_width", width);

    emit maxImageWidthChanged();
}



bool Settings::autoloadImages() const
{
    return _settings.value("autoload_images", true).toBool();
}



void Settings::setAutoloadImages(bool load)
{
    if (load == autoloadImages())
        return;

    _settings.setValue("autoload_images", load);

    emit autoloadImagesChanged(load);
}



bool Settings::hideShortPosts() const
{
    return _settings.value("hide_short_posts").toBool();
}



void Settings::setHideShortPosts(bool hide)
{
    if (hide == hideShortPosts())
        return;

    _settings.setValue("hide_short_posts", hide);

    emit hideShortPostsChanged();
}



bool Settings::hideNegativeRated() const
{
    return _settings.value("hide_negative_rated").toBool();
}



void Settings::setHideNegativeRated(bool hide)
{
    if (hide == hideNegativeRated())
        return;

    _settings.setValue("hide_negative_rated", hide);

    emit hideNegativeRatedChanged();
}



bool Settings::darkTheme() const
{
    return _settings.value("dark_theme").toBool();
}



void Settings::setDarkTheme(bool dark)
{
    if (dark == darkTheme())
        return;

    _settings.setValue("dark_theme", dark);

    emit darkThemeChanged();
}



bool Settings::systemNotifications() const
{
    return _settings.value("system_notifications", true).toBool();
}



void Settings::setSystemNotifications(bool enable)
{
    if (enable == systemNotifications())
        return;

    _settings.setValue("system_notifications", enable);

    emit systemNotificationsChanged();
}



QString Settings::lastTitle() const
{
    return _settings.value("last_title").toString();
}



void Settings::setLastTitle(const QString title)
{
    if (title == lastTitle())
        return;

    _settings.setValue("last_title", title);

    emit lastTitleChanged();
}



QString Settings::lastText() const
{
    return _settings.value("last_text").toString();
}



void Settings::setLastText(const QString text)
{
    if (text == lastText())
        return;

    _settings.setValue("last_text", text);

    emit lastTextChanged();
}



int Settings::lastPrivacy() const
{
    return _settings.value("last_privacy").toInt();
}



void Settings::setLastPrivacy(int privacy)
{
    if (privacy == lastPrivacy())
        return;

    _settings.setValue("last_privacy", privacy);

    emit lastPrivacyChanged();
}



int Settings::lastPostingTlog() const
{
    return _settings.value("last_posting_tlog").toInt();
}



void Settings::setLastPostingTlog(int tlog)
{
    if (tlog == lastPostingTlog())
        return;

    _settings.setValue("last_posting_tlog", tlog);

    emit lastPostingTlogChanged();
}
