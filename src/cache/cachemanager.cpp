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

#ifdef Q_OS_ANDROID
#   include <QGuiApplication>
#else
#   include <QApplication>
#endif

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



CacheManager *CacheManager::instance(int maxSize, QNetworkAccessManager* web)
{
    static auto manager = new CacheManager(maxSize, web);
    return manager;
}



CachedImage* CacheManager::image(const QString& url)
{
    if (_watcher.isRunning())
        _watcher.waitForFinished();

    CachedImage* image = nullptr;
    if (_images.contains(url))
    {
        image = _images.object(url);
        Q_ASSERT(image);
    }

    if (!image)
    {
        image = new CachedImage(this, url);
        Q_TEST(_images.insert(url, image, 0));

        Q_TEST(connect(image, &CachedImage::fileSizeChanged,
                       this, &CacheManager::_setImageCost, Qt::QueuedConnection));
    }

    image->load();
    return image;
}



void CacheManager::setMaxWidth(int maxWidth)
{
    if (maxWidth < 0)
        return;

    _maxWidth = maxWidth;
    _provider->setCacheSize(qMax(20480, _maxWidth * _maxWidth * 3 * 3 / 1024));
}



bool CacheManager::autoload(int size) const
{
    return _maxLoadSize < 0
        || (size <= _maxLoadSize && size > 0)
        || (_autoloadOverWifi && _web->activeConfiguration().bearerType()
                                    == QNetworkConfiguration::BearerWLAN);
}



void CacheManager::setMaxSize(int size)
{
    _images.setMaxCost(size * 1024 * 1024);
    if (!_images.removedCount())
        return;

    Q_TEST(_db.transaction());
    _deleteRemoved();
    Q_TEST(_db.commit());

    emit sizeChanged();
}



void CacheManager::clear()
{
    auto keys = _images.keys();
    int i = 0;
    for (auto key: keys) //! \todo LruCache::iterator
    {
        _images.object(key)->removeFile();
        if (++i % 100 == 0)
            qApp->processEvents();
    }

    Q_TEST(QDir(_path).removeRecursively());

    QSqlQuery query(_db);
    Q_TEST(query.exec("DELETE FROM images"));

    _maxDbRow = 0;
}



void CacheManager::clearOldVersion()
{
    QtConcurrent::run(this, &CacheManager::_clearOldVersion);
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
        if (!image->fileSize() || image->url().isEmpty())
            continue;

        query.addBindValue(image->url());
        query.addBindValue(image->extension());
        query.addBindValue(image->fileSize());
        query.addBindValue(++_maxDbRow);
        Q_TEST(query.exec());
    }

    _deleteRemoved();

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



void CacheManager::_setImageCost()
{
    auto image = qobject_cast<CachedImage*>(sender());
    Q_ASSERT(image);
    if (!image)
        return;

    //! \todo delete removed
    _images.setCost(image->url(), image->diskSpace());

    emit sizeChanged();
}



CacheManager::CacheManager(int maxSize, QNetworkAccessManager* web)
    : QObject()
    , _images(maxSize * 1024 * 1024)
    , _provider(new CachedImageProvider(this))
    , _web(web ? web : new QNetworkAccessManager(this))
    , _loaded(false)
    , _maxDbRow(0)
    , _maxWidth(0)
    , _maxLoadSize(-1)
    , _autoloadOverWifi(true)
{
    qDebug() << "CacheManager";

    _path = QString("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)).arg("images");

    auto future = QtConcurrent::run(this, &CacheManager::_loadDb);
    _watcher.setFuture(future);

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
        auto url = query.value(1).toString();
        if (url.isEmpty())
            continue;

        auto image = new CachedImage(this, query.value(0).toString(),
                                           url,
                                           query.value(2).toInt());
        image->moveToThread(this->thread());

        Q_TEST(connect(image, &CachedImage::fileSizeChanged,
                       this, &CacheManager::_setImageCost, Qt::QueuedConnection));

        Q_TEST(_images.insert(image->url(), image, image->diskSpace()));
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



void CacheManager::_clearOldVersion()
{
    QDir dir(_path);
    auto files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Size);
    foreach (auto file, files)
    {
        if (!QFile::remove(_path + "/" + file))
            qDebug() << "Could not remove image" << file;
    }

    Q_TEST(QMetaObject::invokeMethod(this, "oldVersionCleared", Qt::QueuedConnection));
}



void CacheManager::_deleteRemoved()
{
    QSqlQuery query(_db);
    Q_TEST(query.prepare("DELETE FROM images WHERE url = ?"));
    auto removed = _images.removedValues();
    foreach (auto image, removed)
    {
        image->removeFile();
        image->deleteLater();

        query.addBindValue(image->url());
        Q_TEST(query.exec());
    }

    query.finish();
}
