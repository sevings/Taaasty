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

#include "cachedimageprovider.h"

#include <QPixmapCache>
#include <QDebug>

#include "cachemanager.h"
#include "cachedimage.h"



CachedImageProvider::CachedImageProvider(CacheManager* man)
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
    , _man(man)
{
    QPixmapCache::setCacheLimit(20480);
}



QPixmap CachedImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(requestedSize);

    auto pixmap = _man->image(id)->pixmap();

    if (size)
        *size = pixmap.size();

    Q_ASSERT(!pixmap.isNull());

    return pixmap;
}
