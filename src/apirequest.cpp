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

    Q_TEST(connect(this, SIGNAL(error(int,QString)), tasty, SIGNAL(error(int,QString))));

    _start();
}



ApiRequest::~ApiRequest()
{
    delete _reply;
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

    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data, &jpe);
    if (jpe.error != QJsonParseError::NoError)
    {
//        qDebug() << "parse error:" << jpe.errorString();
//        qDebug() << "json:" << data;
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
        qDebug() << "Unsupported operation in ApiRequest";
        deleteLater();
        return;
    }

    Q_TEST(connect(_reply, SIGNAL(finished()), this, SLOT(_finished())));
    Q_TEST(connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(_printNetworkError(QNetworkReply::NetworkError))));
}
