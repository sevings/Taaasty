#include "tasty.h"
#include "apirequest.h"

#include <QDebug>



Tasty::Tasty(QObject *parent)
    : QObject(parent)
    , _settings(new Settings(this))
{

}



Tasty::~Tasty()
{
    delete _settings;
}



Tasty* Tasty::instance()
{
    static auto tasty = new Tasty;
    return tasty;
}



void Tasty::authorize(const QString login, const QString password)
{
    qDebug() << "authorize";

    auto data = QString("email=%1&password=%2")
            .arg(login)
            .arg(password);

    auto request = new ApiRequest("sessions.json",
                                  QNetworkAccessManager::PostOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));
}



void Tasty::postEntry(const QString title, const QString content)
{
    auto data = QString("title=%1&text=%2&privacy=%3&tlog_id=%4")
            .arg(title)
            .arg(content)
            .arg("public_with_voting")
            .arg("");

    qDebug() << data;

    auto request = new ApiRequest("entries/text.json",
                                  QNetworkAccessManager::PostOperation, data);

    Q_UNUSED(request);

    //connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));
}

void Tasty::getMe()
{
    auto request = new ApiRequest("users/me.json");
    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readMe(const QJsonObject)));
}



void Tasty::_readAccessToken(const QJsonObject data)
{
    auto apiKey = data.value("api_key").toObject();
    auto accessToken = apiKey.value("access_token").toString();
    auto expiresAt = apiKey.value("expires_at").toString();

    qDebug() << "Access token:" << accessToken;
    qDebug() << "Expires at: " << expiresAt;

    _settings->setAccessToken(accessToken);
    _settings->setExpiresAt(expiresAt);

    emit authorized();
}

void Tasty::_readMe(const QJsonObject data)
{
    qDebug() << data.value("title").toString();
}
