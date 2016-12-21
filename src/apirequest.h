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



class ApiRequest : public QObject
{
    Q_OBJECT
public:
    enum Option
    {
        NoOptions           = 0x0,
        AccessTokenRequired = 0x1,
        ShowMessageOnError  = 0x2,
        AllOptions          = AccessTokenRequired | ShowMessageOnError
    };

    Q_DECLARE_FLAGS(Options, Option)

    ApiRequest(const QString& url, const ApiRequest::Options& options = NoOptions);               
    ApiRequest(const QString& url, const QString& accessToken);
    ~ApiRequest();

    bool isValid() const;
    
    bool get();
    bool post();
    bool put();
    bool deleteResource();    
    
    bool addFormData(const QString& name, int value);
    bool addFormData(const QString& name, const QString& content);
    bool addImage(const QString& fileName);
    
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

private:
    bool _init(const QString& url, const ApiRequest::Options& options);
    void _initReply();
    
    QByteArray      _accessToken;
    QNetworkRequest _request;
    QNetworkReply*  _reply; //-V122
    QHttpMultiPart* _data; //-V122
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ApiRequest::Options) //-V813

#endif // APIREQUEST_H
