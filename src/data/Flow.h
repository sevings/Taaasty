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

#ifndef FLOW_H
#define FLOW_H

#include <QObject>
#include <QJsonObject>

#include "TastyData.h"



class Flow : public TastyData
{
    Q_OBJECT

    Q_PROPERTY(QString name           MEMBER  _name           NOTIFY updated)
    Q_PROPERTY(QString slug           MEMBER  _slug           NOTIFY updated)
    Q_PROPERTY(QString title          MEMBER  _title          NOTIFY updated)
    Q_PROPERTY(QString url            MEMBER  _url            NOTIFY updated)
    Q_PROPERTY(QString picUrl         MEMBER  _pic            NOTIFY updated)
    Q_PROPERTY(bool    isPrivate      MEMBER  _isPrivate      NOTIFY updated)
    Q_PROPERTY(bool    isPremoderate  MEMBER  _isPremoderate  NOTIFY updated)
    Q_PROPERTY(bool    isEditable     MEMBER  _isEditable     NOTIFY updated)
    Q_PROPERTY(bool    isWritable     MEMBER  _isWritable     NOTIFY updated)
    Q_PROPERTY(QString followersCount MEMBER  _followersCount NOTIFY updated)
    Q_PROPERTY(QString entriesCount   MEMBER  _entriesCount   NOTIFY updated)

public:
    explicit Flow(QObject* parent = nullptr);

    Q_INVOKABLE void setId(const int id);

    bool isEditable() const;
    bool isWritable() const;

signals:
    void updated();

public slots:
    void init(const QJsonObject& data);

private:
    QString _name;
    QString _slug;
    QString _title;
    QString _url;
    QString _pic;
    bool    _isPrivate;
    bool    _isPremoderate;
    bool    _isEditable;
    bool    _isWritable;
    QString _followersCount;
    QString _entriesCount;
};

#endif // FLOW_H
