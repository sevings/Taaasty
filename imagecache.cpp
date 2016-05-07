#include "imagecache.h"

#include <QStringList>

#include "cachemanager.h"



ImageCache::ImageCache(QQuickItem *parent)
    : QQuickItem(parent)
    ,  _filename("")
    , image(nullptr)
    , _kbytesReceived(0)
    , _kbytesTotal(0)
{

}



QString ImageCache::source() const
{
    return QUrl::fromLocalFile(_filename).toString();
}



void ImageCache::setSource(QString source)
{
    if (_sourceUrl == source)
        return;

    _sourceUrl = source;
    if (source.length() == 0) {
        _filename.clear();
        _extension.clear();
        emit extensionChanged();
        return;
    }

    emit sourceChanged();

    _kbytesReceived = 0;
    _kbytesTotal = 0;
    emit receivedChanged();
    emit totalChanged();

    CacheManager* manager = CacheManager::instance();
    image = manager->download(source);
    if (image->isAvailable)
        _imageAvailable();
    else {
        emit readyToDownload();
        _changeExtension(source);
    }
}



bool ImageCache::isReadyToDownload() const
{
    return image;
}



int ImageCache::received() const
{
    return _kbytesReceived;
}



int ImageCache::total() const
{
    return _kbytesTotal;
}



bool ImageCache::isDownloading() const
{
    return image && image->isDownloading();
}



QString ImageCache::getExtension() const
{
    return _extension;
}



void ImageCache::download()
{
    if (!image) {
        setSource(_sourceUrl);
        return;
    }
    else if (image->isDownloading())
        return;

    connect(image, SIGNAL(available()),                     this, SLOT(_imageAvailable()));
    connect(image, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_changeBytes(qint64,qint64)));

    image->get();
    emit downloadingChanged();
}



void ImageCache::abortDownload()
{
    if (!isDownloading())
    return;

    image->abort();
    delete image;
    image = nullptr;

    emit downloadingChanged();
}



void ImageCache::_imageAvailable()
{
    _filename = image->filename;
    delete image;
    image = nullptr;

    _changeExtension(_filename);

    emit downloadingChanged();
    emit available();
}



void ImageCache::_changeBytes(qint64 bytesReceived, qint64 bytesTotal)
{
    _kbytesReceived = bytesReceived / 1024;
    _kbytesTotal = bytesTotal / 1024;
    emit receivedChanged();
    emit totalChanged();
}



void ImageCache::_changeExtension(QString url)
{
    auto ext = url.split(".").last();
    if (ext.length() > 5)
        _extension = "unknown";
    else
        _extension = ext;

    emit extensionChanged();
}

