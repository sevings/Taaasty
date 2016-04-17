#include "settings.h"

Settings::Settings(QObject *parent)
    : QObject(parent)
    , _settings(this)
{

}

QString Settings::login() const
{
    return _settings.value("login").toString();
}

void Settings::setLogin(const QString login)
{
    _settings.setValue("login", login);
}

QString Settings::accessToken() const
{
    return _settings.value("access_token").toString();
}

void Settings::setAccessToken(const QString token)
{
    _settings.setValue("access_token", token);
}

QDateTime Settings::expiresAt() const
{
    return _settings.value("expires_at").toDateTime();
}

void Settings::setExpiresAt(const QString date)
{
    _settings.setValue("expires_at", QDateTime::fromString(date.left(19), "yyyy-MM-dd'T'HH:mm:ss"));
}
