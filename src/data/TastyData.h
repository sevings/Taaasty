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

#pragma once

#include <QObject>
#include <QPointer>
#include <QJsonObject>

class ApiRequest;



class TastyData: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int  id              READ id             NOTIFY idChanged)
    Q_PROPERTY(bool loading         READ isLoading      NOTIFY loadingChanged)
    Q_PROPERTY(bool networkError    READ networkError   NOTIFY networkErrorChanged)
    Q_PROPERTY(QString errorString  READ errorString    NOTIFY errorStringChanged)

public:
    TastyData(QObject* parent = nullptr);

    void    setCppOwnership();

    int     id() const;
    bool    isLoading() const;
    bool    networkError() const;
    QString errorString() const;

signals:
    void idChanged();
    void loadingChanged();
    void errorStringChanged();
    void networkErrorChanged();

protected slots:
    void _setErrorString(int errorCode, QString str);
    void _setStringValue(QString& string, const QJsonObject& data, const char* name);

protected:
    int _id;
    QPointer<ApiRequest> _request;
    bool                 _networkError;
    QString              _errorString;

    void _initRequest();
};
