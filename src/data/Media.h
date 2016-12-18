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

class AttachedImage;



class Media: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString        url       MEMBER _url       CONSTANT)
    Q_PROPERTY(QString        title     MEMBER _title     CONSTANT)
    Q_PROPERTY(int            duration  MEMBER _duration  CONSTANT)
    Q_PROPERTY(AttachedImage* thumbnail MEMBER _thumbnail CONSTANT)
    Q_PROPERTY(AttachedImage* icon      MEMBER _icon      CONSTANT)

public:
    Media(QObject* parent = nullptr);
    Media(const QJsonObject& data, QObject* parent = nullptr);

private:
    QString _url;
    QString _title;
    int     _duration;

    AttachedImage* _thumbnail; //-V122
    AttachedImage* _icon;      //-V122

};
