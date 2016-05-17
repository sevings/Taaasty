#include "settings.h"

#include <QGuiApplication>
#include <QScreen>



Settings::Settings(QObject *parent)
    : QObject(parent)
    , _settings(this)
{
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
    _settings.setValue("login", login);

    emit loginChanged();
}



QString Settings::accessToken() const
{
    return _settings.value("access_token").toString();
}



void Settings::setAccessToken(const QString token)
{
    _settings.setValue("access_token", token);

    emit accessTokenChanged();
}



QDateTime Settings::expiresAt() const
{
    return _settings.value("expires_at").toDateTime();
}



void Settings::setExpiresAt(const QString date)
{
    _settings.setValue("expires_at", QDateTime::fromString(date.left(19), "yyyy-MM-ddTHH:mm:ss"));
}



int Settings::maxImageWidth() const
{
    return _settings.value("max_image_width").toInt();
}



void Settings::setMaxImageWidth(const int width)
{
    _settings.setValue("max_image_width", width);

    emit maxImageWidthChanged();
}



bool Settings::autoloadImages() const
{
    return _settings.value("autoload_images").toBool();
}



void Settings::setAutoloadImages(bool load)
{
    _settings.setValue("autoload_images", load);

    emit autoloadImagesChanged(load);
}



bool Settings::hideShortPosts() const
{
    return _settings.value("hide_short_posts").toBool();
}



void Settings::setHideShortPosts(bool hide)
{
    _settings.setValue("hide_short_posts", hide);

    emit hideShortPostsChanged();
}
