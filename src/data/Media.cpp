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

#include "Media.h"

#include <QJsonArray>

#include "../defines.h"

#include "AttachedImage.h"



Media::Media(QObject* parent)
    : QObject(parent)
    , _duration(0)
    , _thumbnail(new AttachedImage(this))
    , _icon(new AttachedImage(this))
{

}



Media::Media(const QJsonObject& data, QObject* parent)
    : QObject(parent)
{
    auto meta = data.value(QStringLiteral("meta")).toObject();
    _title    = meta.value(QStringLiteral("title")).toString();
    _duration = meta.value(QStringLiteral("duration")).toInt();

    auto links = data.value(QStringLiteral("links")).toObject();
    _url = links.value(QStringLiteral("player")).toArray().first().toObject().value(QStringLiteral("href")).toString();

    auto imageData = links.contains(QStringLiteral("thumbnail"))
            ? links.value(QStringLiteral("thumbnail")) : links.value(QStringLiteral("image"));
    auto thumb = imageData.toArray().first().toObject();
    _thumbnail = new AttachedImage(thumb, this);

    auto icon = links.value(QStringLiteral("icon")).toArray().first().toObject();
    _icon     = new AttachedImage(icon, this);
}
