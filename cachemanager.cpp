#include "cachemanager.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QStandardPaths>



CacheManager *CacheManager::instance()
{
    static auto manager = new CacheManager;
    return manager;
}



CacheManager::CacheManager()
{
    _path = QDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    _path.mkpath("images");
    _path.cd("images");

    _web = Tasty::instance()->manager();
}



ImagePath *CacheManager::download(QString url)
{
    auto image = new ImagePath(_web, _path, url);
    return image;
}



ImagePath::ImagePath(QNetworkAccessManager* web, QDir path, QString url)
    : isAvailable(false)
    , _reply(nullptr)
    , _web(web)
    , _url(url)
{
    filename = path.absoluteFilePath(QString("%1.jpg").arg(qHash(url)));

    _checkExists();
}



void ImagePath::get()
{
    if (isAvailable) {
        emit available();
        return;
    }

    if (_checkExists())
        return;

    _reply = _web->get(QNetworkRequest(_url));
    connect(_reply, SIGNAL(finished()),                      this, SLOT(_saveFile()));
    connect(_reply, SIGNAL(sslErrors(QList<QSslError>)),     this, SLOT(_ignoreSslErrors()));
    connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_emitProgressChanged(qint64,qint64)));
}



bool ImagePath::isDownloading()
{
    return _reply && _reply->isRunning();
}



void ImagePath::abort()
{
    if (!isDownloading())
        return;

    _reply->abort();
    _reply->deleteLater();
    _reply = nullptr;
}



void ImagePath::_saveFile() {
    if (_reply->error() != QNetworkReply::NoError ) {
        if (_reply->error() == QNetworkReply::OperationCanceledError)
            return;

        _reply->deleteLater();
        _reply = nullptr;

        get();
        return; // ?
    }

    int period = filename.lastIndexOf('.');
    if (period > 0) {
        filename.truncate(period + 1);
    }
    else
        filename.append('.');

    QString suffix;
    auto img = _reply->readAll();
    if (img.startsWith(0x89))
        suffix = "png";
    else if (img.startsWith(0xFF))
        suffix = "jpg";
    else if (img.startsWith(0x47))
        suffix = "gif";
    filename.append(suffix);

    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(img);
    file.close();

    _reply->deleteLater();
    _reply = NULL;
    emit available();
}



void ImagePath::_emitProgressChanged(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}



void ImagePath::_ignoreSslErrors()
{
    _reply->ignoreSslErrors();
}



bool ImagePath::_isExists()
{
    if (!QFile::exists(filename))
        return false;

    isAvailable = true;
    emit available();
    return true;
}



bool ImagePath::_checkExists()
{
    filename.replace(filename.length()-3, 3, "jpg");
    if (_isExists())
        return true;
    filename.replace(filename.length()-3, 3, "png");
    if (_isExists())
        return true;
    filename.replace(filename.length()-3, 3, "gif");
    if (_isExists())
        return true;
    return false;
}
