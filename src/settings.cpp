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



void Settings::setExpiresAt(const QString date)
{
    auto at = QDateTime::fromString(date.left(19), "yyyy-MM-ddTHH:mm:ss");
    if (at == expiresAt())
        return;

    _settings.setValue("expires_at", at);
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
