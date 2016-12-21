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
// #include <QImage>
#include <QFileInfo>
#include <QFile>

#include "defines.h"
#include "tasty.h"


ApiRequest::ApiRequest(const QString& url,
                       const ApiRequest::Options& options)
    : _accessToken(pTasty->settings()->accessToken().toUtf8())
    , _reply(nullptr)
    , _data(nullptr)
{
    _init(url, options);
}



ApiRequest::ApiRequest(const QString& url,
                       const QString& accessToken)
    : _accessToken(accessToken.toUtf8())
    , _reply(nullptr)
    , _data(nullptr)
{
    _init(url, AccessTokenRequired);
}



ApiRequest::~ApiRequest()
{
    Q_ASSERT(_reply);

    delete _reply;
}



bool ApiRequest::isValid() const
{
    return !_request.url().isEmpty();
}



bool ApiRequest::get()
{
    Q_ASSERT(!_reply);
    Q_ASSERT(!_data);
    
    if (_reply || !isValid())
        return false;
    
    _reply = pTasty->manager()->get(_request);
    
    _initReply();     
    
    return true;
}



bool ApiRequest::post()
{
    Q_ASSERT(!_reply);
    
    if (_reply || !isValid())
        return false;

    if (_data)
        _reply = pTasty->manager()->post(_request, _data);
    else
        _reply = pTasty->manager()->post(_request, QByteArray());
    
    _initReply();     
    
    return true;
}



bool ApiRequest::put()
{
    Q_ASSERT(!_reply);
    
    if (_reply || !isValid())
        return false;

    if (_data)
        _reply = pTasty->manager()->put(_request, _data);
    else
        _reply = pTasty->manager()->put(_request, QByteArray());
    
    _initReply();     
    
    return true;
}



bool ApiRequest::deleteResource()
{
    Q_ASSERT(!_reply);
    Q_ASSERT(!_data);
    
    if (_reply || !isValid())
        return false;
    
    _reply = pTasty->manager()->deleteResource(_request);
    
    _initReply();    
    
    return true;
}



bool ApiRequest::addFormData(const QString& name, int value)
{
    return addFormData(name, QString::number(value));
}



bool ApiRequest::addFormData(const QString& name, const QString& content)
{
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
        QString("form-data; name=\"%1\"").arg(name));
    part.setBody(content.toUtf8());

    if (!_data)
        _data = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    
    _data->append(part);
    
    return true;
}
    


bool ApiRequest::addImage(const QString& fileName)
{
    QFileInfo info(fileName);
    auto format = info.suffix();
    if (format.isEmpty())
        format = "jpg";
    
#if 0
    QImage pic(fileName);
    if (pic.isNull())
        return false;
    
    if (pic.width() > _man->maxWidth())
        pic = pic.scaledToWidth(_man->maxWidth(), Qt::SmoothTransformation);

    QByteArray body;
    QBuffer buffer(body);
    buffer.open(QIODevice::WriteOnly);    
    pic.save(&buffer, format.toUtf8().constData());
#endif
    
    QFile* file = new QFile(fileName, this);
    if(!file->open(QIODevice::ReadOnly))
        return false;
    
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QString("image/%1").arg(format));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
        QVariant(QString("form-data; name=\"files[]\"; filename=\"%1\"").arg(info.fileName())));
    // imagePart.setBody(body);
    imagePart.setBodyDevice(file);

    if (!_data)
        _data = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    
    _data->append(imagePart);
    
    return true;
}
   


void ApiRequest::_printNetworkError(QNetworkReply::NetworkError code)
{
#ifdef QT_DEBUG
    if (_reply)
        qDebug() << code << _reply->errorString(); //AuthenticationRequiredError 204, UnknownContentError 299, Server Time-Out 499
    else
        qDebug() << code;
#endif

    emit networkError(code);

    deleteLater();
}



void ApiRequest::_handleResult()
{
    deleteLater();

    if (!_reply)
        return;

    auto data = _reply->readAll();

    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data, &jpe);
    if (jpe.error != QJsonParseError::NoError)
    {
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
        auto errorString = jsonObject.contains("long_message") ? jsonObject.value("long_message").toString()
                                                               : jsonObject.value("error").toString();
        auto code = jsonObject.value("response_code").toInt();

        emit error(code, errorString);

        qDebug() << jsonObject;
    }
}



bool ApiRequest::_init(const QString& url, const ApiRequest::Options& options)
{
    qDebug() << "ApiRequest to" << url;

    if ((options & AccessTokenRequired)
            && _accessToken.isEmpty())// || expiresAt <= QDateTime::currentDateTime()))
    {
        qDebug() << "authorization needed for" << url;
        emit pTasty->authorizationNeeded();
        deleteLater();
        return false;
    }

    if (options & ShowMessageOnError)
    {
        Q_TEST(connect(this, SIGNAL(error(int,QString)),
                       pTasty, SIGNAL(error(int,QString))));
                       
        Q_TEST(connect(this, &ApiRequest::networkError, []()
        {
            emit pTasty->error(0, "Сетевая ошибка");
        }));
    }    
    
    _request.setUrl(QString("http://api.taaasty.com:80/").append(url));
    _request.setRawHeader(QByteArray("X-User-Token"), _accessToken);

    QTimer::singleShot(120000, this, &QObject::deleteLater);

    return true;
}



void ApiRequest::_initReply()
{
    if (!_reply)
        return;

    Q_TEST(connect(_reply, static_cast<void(QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                   this, &ApiRequest::_printNetworkError));
    
    Q_TEST(connect(_reply, &QNetworkReply::finished,         this, &ApiRequest::_handleResult));

    Q_TEST(connect(_reply, &QNetworkReply::downloadProgress, this, &ApiRequest::progress));
    Q_TEST(connect(_reply, &QNetworkReply::uploadProgress,   this, &ApiRequest::progress));
}
