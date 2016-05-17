#include "apirequest.h"

#include <QNetworkRequest>
#include <QJsonParseError>

#include "defines.h"



ApiRequest::ApiRequest(const QString url,
                       const bool accessTokenRequired,
                       const QNetworkAccessManager::Operation method,
                       const QString data)
    : _reply(nullptr)
{
    QUrl fullUrl(QString("http://api.taaasty.com:80/v1/%1").arg(url));
    auto readyData = data.toUtf8();

    auto tasty = Tasty::instance();
    auto settings = tasty->settings();
    auto accessToken = settings->accessToken();
    //auto expiresAt = settings->expiresAt();

    if (accessTokenRequired && (!tasty->isAuthorized()))// || expiresAt <= QDateTime::currentDateTime()))
    {
        qDebug() << "authorization needed for" << url;
        emit tasty->authorizationNeeded();
        deleteLater();
        return;
    }

    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, readyData.length());
    request.setRawHeader(QByteArray("X-User-Token"), accessToken.toUtf8());
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));

    tasty->incBusy();

    auto manager = tasty->manager();
    switch(method)
    {
    case QNetworkAccessManager::GetOperation:
        _reply = manager->get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        _reply = manager->put(request, readyData);
        break;
    case QNetworkAccessManager::PostOperation:
        _reply = manager->post(request, readyData);
        break;
    case QNetworkAccessManager::DeleteOperation:
        _reply = manager->deleteResource(request);
        break;
    default:
        qDebug() << "Unsopperted operation in ApiRequest";
        deleteLater();
        return;
    }

    Q_TEST(connect(_reply, SIGNAL(finished()), this, SLOT(_finished())));
    Q_TEST(connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(_printNetworkError(QNetworkReply::NetworkError))));
}



ApiRequest::~ApiRequest()
{
    delete _reply;

    Tasty::instance()->decBusy();
}



void ApiRequest::_printNetworkError(QNetworkReply::NetworkError code)
{
    qDebug() << code << _reply->errorString(); //AuthenticationRequiredError 204, UnknownContentError 299

    emit error(code);

    deleteLater();
}



void ApiRequest::_finished()
{
    deleteLater();

    auto data = _reply->readAll();

    QJsonParseError error;
    auto json = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) //! TODO: emit signal
    {
        qDebug() << error.errorString();
        return;
    }

    auto jsonObject = json.object();
    if (_reply->error() == QNetworkReply::NoError)
        emit success(jsonObject);
    else
    {
        Tasty::instance()->showError(jsonObject);
        qDebug() << jsonObject;
    }
}
