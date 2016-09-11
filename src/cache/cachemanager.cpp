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

#include "cachemanager.h"

#include <QNetworkAccessManager>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include <QSslSocket>

#include "cachedimage.h"



CacheManager *CacheManager::instance(QNetworkAccessManager* web)
{
    static auto manager = new CacheManager(web);
    return manager;
}



CacheManager::CacheManager(QNetworkAccessManager* web)
    : _maxWidth(0)
    , _autoload(true)
{
    qDebug() << "CacheManager";

    _web = web ? web : new QNetworkAccessManager(this);

    Q_ASSERT(QSslSocket::supportsSsl());
}



QString CacheManager::path() const
{
    return _path;
}



QNetworkAccessManager* CacheManager::web() const
{
    return _web;
}



bool CacheManager::autoload() const
{
    return _autoload;
}



void CacheManager::setAutoload(bool autoload)
{
    _autoload = autoload;
}



int CacheManager::maxWidth() const
{
    return _maxWidth;
}



void CacheManager::setMaxWidth(int maxWidth)
{
    if (maxWidth >= 0)
        _maxWidth = maxWidth;
}



CachedImage* CacheManager::image(QString url)
{
    if (_images.contains(url))
    {
        auto image = _images[url];
        if (_autoload && !image->isAvailable())
            image->download();
        return image;
    }

    if (_path.isEmpty())
    {
        auto cachePath = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
        cachePath.mkpath("images");
        cachePath.cd("images");
        _path = cachePath.absolutePath();
    }

    auto image = new CachedImage(this, url);
    _images.insert(url, image);
    return image;
}
