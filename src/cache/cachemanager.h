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

#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QFutureWatcher>
#include <QSqlDatabase>

#include "lrucache.hpp"

class QNetworkAccessManager;
class CachedImage;
class CachedImageProvider;



class CacheManager : public QObject
{
    Q_OBJECT

public:
    ~CacheManager();

    static CacheManager* instance(QNetworkAccessManager* web = nullptr);

    Q_INVOKABLE CachedImage* image(const QString& url = QString());

    CachedImageProvider* provider() const;
    QNetworkAccessManager* web() const;
    QString path() const;

    int  maxWidth() const;
    void setMaxWidth(int maxWidth);

    bool autoloadOverWifi() const;
    int  maxLoadSize() const;

    bool autoload(int size = 0) const;

public slots:
    void setAutoloadOverWifi(bool load);
    void setMaxLoadSize(int size);

    void clearUnusedImages();

    void saveDb();

private slots:
    void _insertAvailableImage();

private:
    CacheManager(QNetworkAccessManager* web = nullptr);

    void _initDb();
    void _loadDb();

    void _setPath();
    void _clearUnusedImages();
    void _clearOldVersion();

    LruCache<QString, CachedImage>  _images;

    CachedImageProvider*            _provider; //-V122
    QNetworkAccessManager*          _web; //-V122
    QFutureWatcher<void>            _watcher;
    QString                         _path;

    QSqlDatabase                    _db;
    bool                            _loaded;
    quint64                         _maxDbRow;

    int                             _maxWidth;
    int                             _maxLoadSize;
    bool                            _autoloadOverWifi;
};

#endif // CACHEMANAGER_H
