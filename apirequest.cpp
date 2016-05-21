#include "apirequest.h"

#include <QNetworkRequest>
#include <QJsonParseError>

#include "defines.h"



ApiRequest::ApiRequest(const QString url,
                       const bool accessTokenRequired,
                       const QNetworkAccessManager::Operation method,
                       const QString data)
    : _reply(nullptr)
    , _readyData(data.toUtf8())
    , _method(method)
{
    QUrl fullUrl(QString("http://api.taaasty.com:80/v1/%1").arg(url));

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

    start();
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

    auto manager = Tasty::instance()->manager();
    if (manager->networkAccessible() == QNetworkAccessManager::NotAccessible)
    {
        Q_TEST(connect(manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
                                            this, SLOT(_restart(QNetworkAccessManager::NetworkAccessibility))));
    }
    else
        deleteLater();
}



void ApiRequest::_restart(QNetworkAccessManager::NetworkAccessibility na)
{
    if (na == QNetworkAccessManager::Accessible)
        start();
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



void ApiRequest::start()
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
