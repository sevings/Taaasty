#include "cachemanager.h"

#include <QStandardPaths>



CacheManager *CacheManager::Instance()
{
    static CacheManager* manager = new CacheManager;
    return manager;
}

CacheManager::CacheManager()
{
    path = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    path.mkpath("images");
    path.cd("images");

    qDebug() << path.absolutePath();

    web = Tasty::instance()->manager();
}

ImagePath *CacheManager::download(QString url)
{
    ImagePath* image = new ImagePath(web, path, url);
    return image;
}

ImagePath::ImagePath(QNetworkAccessManager* web, QDir path, QString url)
    : isAvailable(false), reply(NULL), web(web), url(url)
{
    filename = path.absoluteFilePath(QString("%1.jpg").arg(qHash(url)));
    checkExists();
}

void ImagePath::get()
{
    if (isAvailable) {
        emit available();
        return;
    }
    if (checkExists())
        return;
    reply = web->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(ignoreSslErrors()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgressChanged(qint64,qint64)));
}

bool ImagePath::isDownloading()
{
    return reply != NULL && reply->isRunning();
}

void ImagePath::abort()
{
    if (isDownloading()) {
        reply->abort();
        reply->deleteLater();
        reply = NULL;
    }
}

void ImagePath::downloadFinished() {
    if (reply->error() != QNetworkReply::NoError ) {
        if (reply->error() == QNetworkReply::OperationCanceledError)
            return;
        reply->deleteLater();
        reply = NULL;
        get();
    }
    int period = filename.lastIndexOf('.');
    if (period > 0) {
        filename.truncate(period + 1);
    }
    else
        filename.append('.');

    QString suffix;
    QByteArray img = reply->readAll();
    if (img.startsWith(0x89))
        suffix = "png";
    if (img.startsWith(0xFF))
        suffix = "jpg";
    if (img.startsWith(0x47))
        suffix = "gif";
    filename.append(suffix);

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(img);
    file.close();

    reply->deleteLater();
    reply = NULL;
    emit available();
}

void ImagePath::downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void ImagePath::ignoreSslErrors()
{
    reply->ignoreSslErrors();
}

bool ImagePath::exists()
{
    if (QFile::exists(filename)) {
        isAvailable = true;
        emit available();
        return true;
    }
    else
        return false;
}

bool ImagePath::checkExists()
{
    filename.replace(filename.length()-3, 3, "jpg");
    if (exists())
        return true;
    filename.replace(filename.length()-3, 3, "png");
    if (exists())
        return true;
    filename.replace(filename.length()-3, 3, "gif");
    if (exists())
        return true;
    return false;
}
