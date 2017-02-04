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

#include "models/uploadmodel.h"

#include "defines.h"
#include "tasty.h"



ApiRequest::ApiRequest(const ApiRequest::Options& options)
    : _reply(nullptr)
    , _data(nullptr)
    , _model(nullptr)
{
    _setOptions(pTasty->settings()->accessToken(), options);
}



ApiRequest::ApiRequest(const QString& url,
                       const ApiRequest::Options& options)
    : _reply(nullptr)
    , _data(nullptr)
    , _model(nullptr)
{
    _setOptions(pTasty->settings()->accessToken(), options);    
    setUrl(url);
}



ApiRequest::ApiRequest(const QString& url,
                       const QString& accessToken,
                       const ApiRequest::Options& options)
    : _reply(nullptr)
    , _data(nullptr)
    , _model(nullptr)
{
    _setOptions(accessToken, options);    
    setUrl(url);
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



bool ApiRequest::isRunning() const
{
    return (_reply && _reply->isRunning()) || (_model && _model->isLoading());
}



bool ApiRequest::setUrl(const QString& url)
{
    Q_ASSERT(!isRunning());
    if (isRunning())
        return false;

    qDebug() << "ApiRequest to" << url;

    _request.setUrl(QStringLiteral("http://api.taaasty.com:80/").append(url));
    return true;
}


    
bool ApiRequest::addFormData(const QString& name, int value)
{
    return addFormData(name, QString::number(value));
}



bool ApiRequest::addFormData(const QString& name, const QString& content)
{
    if (isRunning())
        return false;
    
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
        QString("form-data; name=\"%1\"").arg(name));
    part.setBody(content.toUtf8());

    if (!_data)
        _data = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);
    
    _data->append(part);
    
    return true;
}



bool ApiRequest::addImages(UploadModel* model)
{
    if (isRunning() || !model)
        return false;

    _model = model;

    _model->setParent(this);

    Q_TEST(connect(_model, &UploadModel::loaded, this, &ApiRequest::_addImages));

    _model->loadFiles();

    return true;
}



bool ApiRequest::get()
{
    Q_ASSERT(!isRunning());
    Q_ASSERT(!_data);

    if (isRunning() || !isValid())
        return false;

    _reply = pTasty->manager()->get(_request);

    _initReply();

    return true;
}



bool ApiRequest::post()
{
    if (!isValid() || _reply)
        return false;

    if (_model && _model->isLoading())
    {
        Q_TEST(connect(_model, &UploadModel::loaded, this, &ApiRequest::post, Qt::QueuedConnection));
        return true;
    }

    if (_data)
        _reply = pTasty->manager()->post(_request, _data);
    else
        _reply = pTasty->manager()->post(_request, QByteArray());

    _initReply();

    return true;
}



bool ApiRequest::put()
{
    if (!isValid() || _reply)
        return false;

    if (_model && _model->isLoading())
    {
        Q_TEST(connect(_model, &UploadModel::loaded, this, &ApiRequest::put, Qt::QueuedConnection));
        return true;
    }

    if (_data)
        _reply = pTasty->manager()->put(_request, _data);
    else
        _reply = pTasty->manager()->put(_request, QByteArray());

    _initReply();

    return true;
}



bool ApiRequest::deleteResource()
{
    Q_ASSERT(!isRunning());
    Q_ASSERT(!_data);

    if (isRunning() || !isValid())
        return false;

    _reply = pTasty->manager()->deleteResource(_request);

    _initReply();

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
        auto errorString = jsonObject.contains(QStringLiteral("long_message")) ? jsonObject.value(QStringLiteral("long_message")).toString()
                                                               : jsonObject.value(QStringLiteral("error")).toString();
        auto code = jsonObject.value(QStringLiteral("response_code")).toInt();

        emit error(code, errorString);

        qDebug() << jsonObject;
    }
}



void ApiRequest::_addImages()
{
    auto& parts = _model->parts();
    if (parts.isEmpty())
        return;

    if (!_data)
        _data = new QHttpMultiPart(QHttpMultiPart::FormDataType, this);

    foreach (auto part, parts)
        _data->append(part);

    _model->clear();
}



bool ApiRequest::_setOptions(const QString& accessToken, const ApiRequest::Options& options)
{
    if ((options & AccessTokenRequired)
            && accessToken.isEmpty())// || expiresAt <= QDateTime::currentDateTime()))
    {
        qDebug() << "authorization needed for" << _request.url();
        emit pTasty->authorizationNeeded();
        deleteLater();
        return false;
    }

    if (options & ShowTastyError)
        Q_TEST(connect(this, SIGNAL(error(int,QString)),
                       pTasty, SIGNAL(error(int,QString))));
                       
    if (options & ShowNetworkError)
        Q_TEST(connect(this, &ApiRequest::networkError, []()
        {
            emit pTasty->error(0, "Сетевая ошибка");
        }));    
    
    _request.setRawHeader(QByteArrayLiteral("X-User-Token"), accessToken.toUtf8());
    
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

    // QTimer::singleShot(120000, this, &QObject::deleteLater);
}
