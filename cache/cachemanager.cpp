#include "cachemanager.h"

#include <QNetworkAccessManager>
#include <QStandardPaths>
#include <QDir>

#include "cachedimage.h"



CacheManager *CacheManager::instance(QNetworkAccessManager* web)
{
    static auto manager = new CacheManager(web);
    return manager;
}



CacheManager::CacheManager(QNetworkAccessManager* web)
{
    auto cachePath = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    cachePath.mkpath("images");
    cachePath.cd("images");
    _path = cachePath.absolutePath();

    _web = web ? web : new QNetworkAccessManager(this);
}



CachedImage* CacheManager::image(QString url)
{
    if (_images.contains(url))
        return _images[url];

    auto image = new CachedImage(this, url);
    _images.insert(url, image);
    return image;
}
