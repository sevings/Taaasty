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
