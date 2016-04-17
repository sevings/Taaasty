#include "apirequest.h"

#include <QNetworkRequest>
#include <QJsonParseError>



ApiRequest::ApiRequest(const QString url,
                       const QNetworkAccessManager::Operation method,
                       const QString data)
    : _reply(nullptr)
{
    QUrl fullUrl(QString("http://api.taaasty.com:80/v1/%1").arg(url));
    auto readyData = data.toUtf8();

    auto tasty = Tasty::instance();
    auto settings = tasty->settings();
    auto accessToken = settings->accessToken();
    auto expiresAt = settings->expiresAt();

    if ((accessToken.isEmpty() || expiresAt <= QDateTime::currentDateTime()) && !url.startsWith("sessions"))
    {
        qDebug() << "authorization needed";
        emit tasty->authorizationNeeded();
        deleteLater();
        return;
    }

    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, readyData.length());
    request.setRawHeader(QByteArray("X-User-Token"), accessToken.toUtf8());
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));

    switch(method)
    {
    case QNetworkAccessManager::GetOperation:
        _reply = _manager()->get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        _reply = _manager()->put(request, readyData);
        break;
    case QNetworkAccessManager::PostOperation:
        _reply = _manager()->post(request, readyData);
        break;
    case QNetworkAccessManager::DeleteOperation:
        _reply = _manager()->deleteResource(request);
        break;
    default:
        qDebug() << "Unsopperted operation in ApiRequest";
        deleteLater();
        return;
    }

    connect(_reply, SIGNAL(finished()), this, SLOT(_finished()));
    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(_printNetworkError(QNetworkReply::NetworkError)));
}



ApiRequest::~ApiRequest()
{
    delete _reply;
}



void ApiRequest::_printNetworkError(QNetworkReply::NetworkError code)
{
    qDebug() << code << _reply->errorString();

    emit error(code);
}



void ApiRequest::_finished()
{
    deleteLater();

    if (_reply->error() != QNetworkReply::NoError)
        return;

    auto data = _reply->readAll();

    QJsonParseError error;
    auto json = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) //! TODO: emit signal
    {
        qDebug() << error.errorString();
        return;
    }

    auto jsonObject = json.object();
    emit success(jsonObject);
}



QNetworkAccessManager *ApiRequest::_manager() const
{
    static auto manager = new QNetworkAccessManager;
    return manager;
}
