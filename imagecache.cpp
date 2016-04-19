#include "imagecache.h"

ImageCache::ImageCache(QQuickItem *parent) :
    QQuickItem(parent),  filename(""), image(NULL), kbytesReceived(0), kbytesTotal(0), extension()
{

}

QString ImageCache::source() {
    return QUrl::fromLocalFile(filename).toString();
}

void ImageCache::setSource(QString source) {
    sourceUrl = source;
    if (source.length() == 0) {
        filename.clear();
        extension = "";
        emit extensionChanged();
        return;
    }
    emit sourceChanged();
    kbytesReceived = 0;
    kbytesTotal = 0;
    emit receivedChanged();
    emit totalChanged();
    CacheManager* manager = CacheManager::Instance();
    image = manager->download(source);
    if (image->isAvailable)
        imageAvailable();
    else {
        emit readyToDownload();
        changeExtension(source);
    }
}

bool ImageCache::isReadyToDownload()
{
    return image != NULL;
}

int ImageCache::received()
{
    return kbytesReceived;
}

int ImageCache::total()
{
    return kbytesTotal;
}

bool ImageCache::isDownloading()
{
    return image != NULL && image->isDownloading();
}

QString ImageCache::getExtension()
{
    return extension;
}

void ImageCache::download()
{
    if (image == NULL) {
        setSource(sourceUrl);
        return;
    }
    else if (image->isDownloading())
        return;
    connect(image, SIGNAL(available()), this, SLOT(imageAvailable()));
    connect(image, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(changeBytes(qint64,qint64)));
    image->get();
    emit downloadingChanged();
}

void ImageCache::abortDownload()
{
    if(isDownloading()) {
        image->abort();
        delete image;
        image = NULL;
        emit downloadingChanged();
    }
}

void ImageCache::imageAvailable()
{
    filename = image->filename;
    delete image;
    image = NULL;
    emit available();
    emit downloadingChanged();
    changeExtension(filename);
}

void ImageCache::changeBytes(qint64 bytesReceived, qint64 bytesTotal)
{
    kbytesReceived = bytesReceived / 1024;
    kbytesTotal = bytesTotal / 1024;
    emit receivedChanged();
    emit totalChanged();
}

void ImageCache::changeExtension(QString url)
{
    QString ext = url.split(".").last();
    if (ext.length() > 5)
        extension = "unknown";
    else
        extension = ext;
    emit extensionChanged();
}

