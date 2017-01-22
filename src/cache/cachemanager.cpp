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
#include <QSqlQuery>
#include <QSqlError>

#ifdef QT_DEBUG
#   include <QDateTime>
#endif

#include <QSslSocket>

#include "../defines.h"
#include "cachedimage.h"
#include "cachedimageprovider.h"

#define BORDER_NAME QStringLiteral("<border_name>")



CacheManager::~CacheManager()
{
//    delete _provider;

    saveDb();
}



CacheManager *CacheManager::instance(QNetworkAccessManager* web)
{
    static auto manager = new CacheManager(web);
    return manager;
}



CacheManager::CacheManager(QNetworkAccessManager* web)
    : QObject()
    , _images(100000)
    , _provider(new CachedImageProvider(this))
    , _web(web ? web : new QNetworkAccessManager(this))
    , _loaded(false)
    , _maxDbRow(0)
    , _maxWidth(0)
    , _maxLoadSize(-1)
    , _autoloadOverWifi(true)
{
    qDebug() << "CacheManager";

    _loadDb();

    Q_ASSERT(QSslSocket::supportsSsl());
}



void CacheManager::_initDb()
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "cache");
    _db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/cache");
    Q_TEST(_db.open());

    QSqlQuery query(_db);
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS images(url TEXT, format TEXT, size INTEGER, row INTEGER, PRIMARY KEY(url))"));
}



void CacheManager::_loadDb()
{
    if (_loaded)
        return;

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    _initDb();

    Q_TEST(_db.transaction());

    QSqlQuery query(_db);
    Q_TEST(query.exec("SELECT url, format, size, row FROM images ORDER BY row ASC"));
    while (query.next())
    {
        auto image = new CachedImage(this, query.value(0).toString(),
                                           query.value(1).toString(),
                                           query.value(2).toInt(),
                                           query.value(3).value<quint64>());
        image->moveToThread(this->thread());
        _images.insert(image->url(), image, image->fileSize());
    }

    _images.insert(BORDER_NAME, new CachedImage(this), 0);

    Q_TEST(query.exec("SELECT MAX(row) FROM images"));
    if (query.next())
        _maxDbRow = query.value(0).value<quint64>();

    query.finish();

    Q_TEST(_db.commit());

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Cache loaded in" << ms << "ms";
#else
    qDebug() << "Cache loaded";
#endif

    _loaded = true;
}



CachedImageProvider* CacheManager::provider() const
{
    return _provider;
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

//    auto future = QtConcurrent::run(this, &CacheManager::_clearUnusedImages);
    //    _watcher.setFuture(future);
}



void CacheManager::saveDb()
{
    if (!_loaded)
        return;

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    auto images = _images.valuesAfter(BORDER_NAME);

    Q_TEST(_db.transaction());

    QSqlQuery query(_db);
    Q_TEST(query.prepare("INSERT OR REPLACE INTO images VALUES (?, ?, ?, ?)"));
    foreach (auto image, images)
    {
        query.addBindValue(image->url());
        query.addBindValue(image->extension());
        query.addBindValue(image->fileSize());
        query.addBindValue(++_maxDbRow);
        Q_TEST(query.exec());
    }

    query.finish();

    Q_TEST(_db.commit());

    if (!_images.object(BORDER_NAME))
        _images.insert(BORDER_NAME, new CachedImage(this), 0);

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Cache saved in" << ms << "ms";
#else
    qDebug() << "Cache saved";
#endif
}



void CacheManager::_insertAvailableImage()
{
    auto image = qobject_cast<CachedImage*>(sender());
    Q_ASSERT(image);
    if (!image || _images.contains(image->url()))
        return;

    _images.insert(image->url(), image, image->fileSize());
}



CachedImage* CacheManager::image(const QString& url)
{
    if (_images.contains(url))
    {
        auto image = _images.object(url);
        if (!image->isAvailable() && autoload(image->total()))
            image->download();
        return image;
    }

    if (_path.isEmpty())
        _setPath();

    if (_watcher.isRunning())
        _watcher.waitForFinished();

    auto image = new CachedImage(this, url);

    Q_TEST(connect(image, &CachedImage::available, this, &CacheManager::_insertAvailableImage));

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

}



void CacheManager::_clearOldVersion()
{
    QDir dir(_path);
    auto files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Size);
    foreach (auto file, files)
    {
        if (!QFile::remove(_path + "/" + file))
            qDebug() << "Could not remove image" << file;
    }
}
