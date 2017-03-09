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

#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QHttpMultiPart>

#include "defines.h"



class ApiRequest : public QObject
{
    Q_OBJECT
public:
    enum Option
    {
        NoOptions           = 0x0,
        AccessTokenRequired = 0x1,
        ShowTastyError      = 0x2,
        ShowNetworkError    = 0x4,
        ShowMessageOnError  = ShowTastyError | ShowNetworkError,
        AllOptions          = AccessTokenRequired | ShowMessageOnError
    };

    Q_DECLARE_FLAGS(Options, Option)

    ApiRequest(const ApiRequest::Options& options = NoOptions);
    ApiRequest(const QString& url, const ApiRequest::Options& options = NoOptions);               
    ApiRequest(const QString& url, const QString& accessToken, 
               const ApiRequest::Options& options = AccessTokenRequired);
    ~ApiRequest();

    bool isValid() const;
    bool isRunning() const;

    bool setUrl(const QString& url);
    
    bool addFormData(const QString& name, int value);
    bool addFormData(const QString& name, const QString& content);
    bool addImages(UploadModelPtr model);
    
public slots:
    bool get();
    bool post();
    bool put();
    bool deleteResource();

signals:
    void progress(qint64 bytes, qint64 bytesTotal);

    void success(const QJsonObject& data);
    void success(const QJsonArray& data);
    void success(const QString& data);

    void networkError(QNetworkReply::NetworkError code);
    void error(const int code, const QString& text);

private slots:
    void _printNetworkError(QNetworkReply::NetworkError code);
    void _handleResult();

    void _addImages();

private:
    bool _setOptions(const QString& accessToken, const ApiRequest::Options& options);
    void _initReply();

    QNetworkRequest _request;
    QNetworkReply*  _reply; //-V122
    QHttpMultiPart* _data; //-V122
    UploadModelPtr  _model; //-V122
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ApiRequest::Options) //-V813

#endif // APIREQUEST_H
