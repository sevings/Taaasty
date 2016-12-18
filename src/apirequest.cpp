// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include <QTimer>

#include "defines.h"



ApiRequest::ApiRequest(const QString& url,
                       const ApiRequest::Options& options,
                       const QNetworkAccessManager::Operation method,
                       const QString& data)
    : _readyData(data.toUtf8())
    , _accessToken(pTasty->settings()->accessToken().toUtf8())
    , _fullUrl(QString("http://api.taaasty.com:80/%1").arg(url))
{
    qDebug() << "ApiRequest to" << url;

    if ((options & AccessTokenRequired)
            && (!pTasty->isAuthorized()))// || expiresAt <= QDateTime::currentDateTime()))
    {
        qDebug() << "authorization needed for" << url;
        emit pTasty->authorizationNeeded();
        deleteLater();
        return;
    }
    
    QTimer::singleShot(60000, this, &QObject::deleteLater);
    
    Q_TEST(connect(pTasty, &Tasty::networkNotAccessible, this, &QObject::deleteLater));

    if (options & ShowMessageOnError)
        Q_TEST(connect(this, SIGNAL(error(int,QString)),
                       pTasty, SIGNAL(error(int,QString))));

    _start(method);
}



ApiRequest::ApiRequest(const QString& url,
                       const QString& accessToken)
    : _accessToken(accessToken.toUtf8())
    , _fullUrl(QString("http://api.taaasty.com:80/%1").arg(url))
{
    qDebug() << "ApiRequest to" << url;

    if (accessToken.isEmpty())
    {
        deleteLater();
        return;
    }

    _start(QNetworkAccessManager::GetOperation);
}



void ApiRequest::_printNetworkError(QNetworkReply::NetworkError code)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());

#ifdef QT_DEBUG
    if (reply)
        qDebug() << code << reply->errorString(); //AuthenticationRequiredError 204, UnknownContentError 299, Server Time-Out 499
    else
        qDebug() << code;
#endif

    emit error(code);

    if (reply)
        emit error(code, reply->errorString()); //-V2006

    deleteLater();
}



void ApiRequest::_finished()
{
    deleteLater();

    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    auto data = reply->readAll();

    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data, &jpe);
    if (jpe.error != QJsonParseError::NoError)
    {
        emit success(QString::fromUtf8(data));
        return;
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        if (json.isObject())
            emit success(json.object());
        else
            emit success(json.array());
    }
    else
    {
        auto jsonObject = json.object();
        auto errorString = jsonObject.contains("long_message") ? jsonObject.value("long_message").toString()
                                                               : jsonObject.value("error").toString();
        auto code = jsonObject.value("response_code").toInt();

        emit error(code, errorString);

        qDebug() << jsonObject;
    }
}



void ApiRequest::_start(const QNetworkAccessManager::Operation method)
{
    QNetworkRequest request;
    request.setUrl(_fullUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, _readyData.length());
    request.setRawHeader(QByteArray("X-User-Token"), _accessToken);
    request.setRawHeader(QByteArray("Connection"), QByteArray("close"));

    QNetworkReply* reply = nullptr;
    auto manager = pTasty->manager();
    switch(method)
    {
    case QNetworkAccessManager::GetOperation:
        reply = manager->get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        reply = manager->put(request, _readyData);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = manager->post(request, _readyData);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = manager->deleteResource(request);
        break;
    default:
        qDebug() << "Unsupported operation in ApiRequest";
        deleteLater();
        return;
    }

    Q_TEST(connect(reply, SIGNAL(finished()), this, SLOT(_finished())));
    Q_TEST(connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                   this, SLOT(_printNetworkError(QNetworkReply::NetworkError))));
}
