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

#include "cachemanager.h"

#include <QNetworkAccessManager>
#include <QNetworkConfiguration>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>

#include <QSslSocket>

#include "../defines.h"
#include "cachedimage.h"



CacheManager *CacheManager::instance(QNetworkAccessManager* web)
{
    static auto manager = new CacheManager(web);
    return manager;
}



CacheManager::CacheManager(QNetworkAccessManager* web)
    : _maxWidth(0)
    , _maxLoadSize(-1)
    , _autoloadOverWifi(true)
{
    qDebug() << "CacheManager";

    _web = web ? web : new QNetworkAccessManager(this);

    Q_ASSERT(QSslSocket::supportsSsl());
}



QNetworkAccessManager* CacheManager::web() const
{
    return _web;
}



QString CacheManager::path() const
{
    return _path;
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



bool CacheManager::autoloadOverWifi() const
{
    return _autoloadOverWifi;
}



int CacheManager::maxLoadSize() const
{
    return _maxLoadSize;
}



bool CacheManager::autoload(int size) const
{
    return _maxLoadSize < 0
        || (size <= _maxLoadSize && size > 0)
        || (_autoloadOverWifi && _web->activeConfiguration().bearerType()
                                    == QNetworkConfiguration::BearerWLAN);
}



void CacheManager::setAutoloadOverWifi(bool autoload)
{
    _autoloadOverWifi = autoload;
}



void CacheManager::setMaxLoadSize(int size)
{
    _maxLoadSize = size;
}



void CacheManager::clearUnusedImages()
{
    if (_path.isEmpty())
        _setPath();

    auto future = QtConcurrent::run(this, &CacheManager::_clearUnusedImages);
    _watcher.setFuture(future);
}



CachedImage* CacheManager::image(const QString& url)
{
    if (_images.contains(url))
    {
        auto image = _images.value(url);
        if (!image->isAvailable() && autoload(image->total()))
            image->download();
        return image;
    }

    if (_path.isEmpty())
        _setPath();

    if (_watcher.isRunning())
        _watcher.waitForFinished();

    auto image = new CachedImage(this, url);
    _images.insert(url, image);
    return image;
}



void CacheManager::_setPath()
{
    auto cachePath = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    cachePath.mkpath("images");
    cachePath.cd("images");
    _path = cachePath.absolutePath();
}



void CacheManager::_clearUnusedImages()
{
    QSet<QString> images;
    foreach (auto image, _images)
        images << image->sourceFileName();

    QDir dir(_path);
    auto files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Size);
    foreach (auto file, files)
    {
        if (images.contains(file))
            continue;

        if (!QFile::remove(_path + "/" + file))
            qDebug() << "Could not remove image" << file;
    }
}
