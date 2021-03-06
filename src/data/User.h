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
#include <QJsonObject>
#include <QUrl>

#include "TastyData.h"

class ApiRequest;
class Author;



class User: public TastyData
{
    Q_OBJECT

    Q_PROPERTY(QUrl    tlogUrl      MEMBER _tlogUrl     NOTIFY updated)
    Q_PROPERTY(QString name         MEMBER _name        NOTIFY updated)
    Q_PROPERTY(QString slug         MEMBER _slug        NOTIFY updated)

    Q_PROPERTY(QUrl    originalPic     MEMBER _originalPic     NOTIFY updated)
    Q_PROPERTY(QUrl    largePic        MEMBER _largePic        NOTIFY updated)
    Q_PROPERTY(QUrl    thumb128        MEMBER _thumb128        NOTIFY updated)
    Q_PROPERTY(QUrl    thumb64         MEMBER _thumb64         NOTIFY updated)
    Q_PROPERTY(QString symbol          MEMBER _symbol          NOTIFY updated)
    Q_PROPERTY(QString backgroundColor MEMBER _backgroundColor NOTIFY updated)
    Q_PROPERTY(QString nameColor       MEMBER _nameColor       NOTIFY updated)

public:
    User(QObject* parent = nullptr);
    User(const QJsonObject& data, QObject* parent = nullptr);

    User(const User& other);

    void    setId(int id);

    QString name() const;
    QString slug() const;

    void  swap(User& other);
    User& operator=(User other);
    User& operator=(Author* other);

signals:
    void updated();

protected slots:
    void _init(const QJsonObject& data);
    void _initFromTlog(const QJsonObject& data);

private:
    QUrl    _tlogUrl;
    QString _name;
    QString _slug;

    QUrl    _originalPic;
    QUrl    _largePic;
    QUrl    _thumb128;
    QUrl    _thumb64;
    QString _symbol;
    QString _backgroundColor;
    QString _nameColor;
};
