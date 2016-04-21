#include "tasty.h"
#include "apirequest.h"

#include <QDebug>



Tasty::Tasty(QObject *parent)
    : QObject(parent)
    , _settings(new Settings(this))
    , _manager(new QNetworkAccessManager(this))
    , _busy(0)
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

void Tasty::incBusy()
{
    _busy++;
    emit busyChanged();
}

void Tasty::decBusy()
{
    if (_busy > 0)
    {
        _busy--;
        emit busyChanged();
    }
}



void Tasty::authorize(const QString login, const QString password)
{
    qDebug() << "authorize";

    auto data = QString("email=%1&password=%2")
            .arg(login)
            .arg(password);

    auto request = new ApiRequest("sessions.json", false,
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

    auto request = new ApiRequest("entries/text.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_UNUSED(request);

    //connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));
}



void Tasty::getMe()
{
    auto request = new ApiRequest("users/me.json", true);
    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readMe(const QJsonObject)));
}



void Tasty::addComment(const int entryId, const QString text)
{
    auto data = QString("entry_id=%1&text=%2").arg(entryId).arg(text);
    auto request = new ApiRequest("comments.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readComment(const QJsonObject)));
}



void Tasty::editComment(const int id, const QString text)
{
    auto url = QString("comments/%1.json").arg(id);
    auto data = QString("text=%1").arg(text);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readComment(const QJsonObject)));
}



void Tasty::removeComment(const int id)
{
    auto url = QString("comments/%1.json").arg(id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);

    //connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readComment(const QJsonObject)));
}



void Tasty::vote(const int entryId)
{
    _vote(entryId, true);
}



void Tasty::unvote(const int entryId)
{
    _vote(entryId, false);
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



void Tasty::_readComment(const QJsonObject data)
{
    Q_UNUSED(data);
}



void Tasty::_readRating(const QJsonObject data)
{
//    qDebug() << data;

    auto request = static_cast<ApiRequest*>(sender());
    if (!request || !_votes.contains(request))
        return;

    auto entryId = _votes.take(request);
    emit ratingChanged(entryId, data);
}



void Tasty::_readMe(const QJsonObject data)
{
    qDebug() << data.value("title").toString();
}



void Tasty::_vote(const int entryId, const bool add)
{
    auto url = QString("entries/%1/votes.json").arg(entryId);
    auto operation = (add ? QNetworkAccessManager::PostOperation
                          : QNetworkAccessManager::DeleteOperation);
    auto request = new ApiRequest(url, true, operation);

    connect(request, SIGNAL(success(const QJsonObject)),
            this, SLOT(_readRating(const QJsonObject)));

    _votes[request] = entryId;
}
