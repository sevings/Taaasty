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

#include "AttachedImage.h"

#include "../defines.h"

#include "Media.h"
#include "Message.h"



AttachedImage::AttachedImage(QObject* parent)
    : QObject(parent)
    , _width(0)
    , _height(0)
{

}



AttachedImage::AttachedImage(const QJsonObject& data, QObject *parent)
    : QObject(parent)
{
    auto image = data.contains(QStringLiteral("image")) ? data.value(QStringLiteral("image")).toObject() : data;
    _url = image.value(QStringLiteral("url")).toString();

    auto type = data.value(QStringLiteral("content_type")).toString();
    _type = type.split(QStringLiteral("/")).last();

    auto geometry = image.value(QStringLiteral("geometry")).toObject();
    _width = geometry.value(QStringLiteral("width")).toInt();
    _height = geometry.value(QStringLiteral("height")).toInt();
}



AttachedImage::AttachedImage(const QJsonObject& data, Media* parent)
    : QObject(parent)
{
    _url = data.value(QStringLiteral("href")).toString();

    auto type = data.value(QStringLiteral("type")).toString();
    _type = type.split(QStringLiteral("/")).last();

    auto geometry = data.value(QStringLiteral("media")).toObject();
    _width = geometry.value(QStringLiteral("width")).toInt();
    _height = geometry.value(QStringLiteral("height")).toInt();
}
