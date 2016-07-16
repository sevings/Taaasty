#include "apirequest.h"

#include <QNetworkRequest>
#include <QJsonParseError>
#include <QDebug>

#include "defines.h"



ApiRequest::ApiRequest(const QString url,
                       const bool accessTokenRequired,
                       const QNetworkAccessManager::Operation method,
                       const QString data)
    : _reply(nullptr)
    , _readyData(data.toUtf8())
    , _method(method)
{
    qDebug() << "ApiRequest to" << url;

    QUrl fullUrl(QString("http://api.taaasty.com:80/%1").arg(url));

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

    _request.setUrl(fullUrl);
    _request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    _request.setHeader(QNetworkRequest::ContentLengthHeader, _readyData.length());
    _request.setRawHeader(QByteArray("X-User-Token"), accessToken.toUtf8());
    _request.setRawHeader(QByteArray("Connection"), QByteArray("close"));

    tasty->incBusy();

    Q_TEST(connect(this, SIGNAL(error(int,QString)), tasty, SIGNAL(error(int,QString))));

    _start();
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



void ApiRequest::_restart(QNetworkAccessManager::NetworkAccessibility na)
{
    if (na == QNetworkAccessManager::Accessible)
        _start();
}



void ApiRequest::_finished()
{
    deleteLater();

    auto data = _reply->readAll();

    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data, &jpe);
    if (jpe.error != QJsonParseError::NoError)
    {
        qDebug() << "parse error: " << jpe.errorString();
        qDebug() << "json:" << data;
        emit success(QString::fromUtf8(data));
        return;
    }

    if (_reply->error() == QNetworkReply::NoError)
    {
        if (json.isObject())
            emit success(json.object());
        else
            emit success(json.array());
    }
    else
    {
        auto jsonObject = json.object();
        auto errorString = jsonObject.value("error").toString();
        auto code = jsonObject.value("response_code").toInt();

        emit error(code, errorString);

        qDebug() << jsonObject;
    }
}



void ApiRequest::_start()
{
    auto manager = Tasty::instance()->manager();
    switch(_method)
    {
    case QNetworkAccessManager::GetOperation:
        _reply = manager->get(_request);
        break;
    case QNetworkAccessManager::PutOperation:
        _reply = manager->put(_request, _readyData);
        break;
    case QNetworkAccessManager::PostOperation:
        _reply = manager->post(_request, _readyData);
        break;
    case QNetworkAccessManager::DeleteOperation:
        _reply = manager->deleteResource(_request);
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
