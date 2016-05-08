#include "cachedimage.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

#include <QDebug>

#include "cachemanager.h"


CachedImage::CachedImage(CacheManager* parent, QString url)
    : QObject(parent)
    , _man(parent)
    , _headReply(nullptr)
    , _reply(nullptr)
    , _format(UnknownFormat)
    , _url(url)
    , _kbytesReceived(0)
    , _kbytesTotal(0)
{
    if (!_man || _url.isEmpty())
        return;

    _hash = qHash(_url);
    if (_exists())
    {
        emit available();
        return;
    }

    getInfo();
}



QString CachedImage::source() const
{
    return QString("%1/%2.%3").arg(_man->_path).arg(_hash).arg(_extension);
}



bool CachedImage::isReadyToDownload() const
{
    return !_headReply && !_reply;
}



bool CachedImage::isDownloading() const
{
    return _reply && _reply->isRunning();
}



void CachedImage::getInfo()
{
    if (_headReply || _reply)
        return;

    _headReply = _man->_web->head(QNetworkRequest(_url));
    _headReply->ignoreSslErrors();

    Q_ASSERT(connect(_headReply, SIGNAL(finished()), this, SLOT(_setProperties()))); // errors?
}



void CachedImage::download()
{
    if (_url.isEmpty() || !isReadyToDownload() || isDownloading())
        return;

    _reply = _man->_web->get(QNetworkRequest(_url));
    _reply->ignoreSslErrors();

    Q_ASSERT(connect(_reply, SIGNAL(finished()),                      this, SLOT(_saveFile())));
    Q_ASSERT(connect(_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_changeBytes(qint64,qint64))));

    emit downloadingChanged();
}



void CachedImage::abortDownload()
{
    if (!isDownloading())
        return;

    _reply->abort();
    _reply->deleteLater();
    _reply = nullptr;

    emit downloadingChanged();
}



void CachedImage::_setProperties()
{
    auto mime = _headReply->header(QNetworkRequest::ContentTypeHeader).toString().split('/');
    if (mime.isEmpty() || mime.first().toLower() != "image")
    {
        qDebug() << "mime:" << mime.first() << "\nurl:" << _url;
        return;
    }

    _setFormat(mime.first());

    auto length = _headReply->header(QNetworkRequest::ContentLengthHeader).toInt();
    _kbytesTotal = length;

    emit totalChanged();

    _headReply->deleteLater();
    _headReply = nullptr;

    emit readyToDownload();
}



void CachedImage::_saveFile()
{
    if (_reply->error() != QNetworkReply::NoError ) {
        if (_reply->error() == QNetworkReply::OperationCanceledError)
            return;

        _reply->deleteLater();
        _reply = nullptr;

        download();
        return;
    }

    auto img = _reply->readAll();
    if (_format == UnknownFormat)
    {
        if (img.startsWith(0x89))
            _setFormat("png");
        else if (img.startsWith(0xFF))
            _setFormat("jpeg");
        else if (img.startsWith(0x47))
            _setFormat("gif");
    }

    // todo: resize

    QFile file(source());
    file.open(QIODevice::WriteOnly);
    file.write(img);
    file.close();

    _reply->deleteLater();
    _reply = nullptr;

    emit available();
}



void CachedImage::_changeBytes(qint64 bytesReceived, qint64 bytesTotal)
{
    _kbytesReceived = bytesReceived / 1024;
    emit receivedChanged();

    _kbytesTotal = bytesTotal / 1024;
    emit totalChanged();
}



bool CachedImage::_exists()
{
    const auto path = QString("%1/%2.%3").arg(_man->_path).arg(_hash);

    if (QFile::exists(path.arg("jpg")))
    {
        _setFormat("jpeg");
        return true;
    }
    if (QFile::exists(path.arg("png")))
    {
        _setFormat("png");
        return true;
    }

    if (QFile::exists(path.arg("gif")))
    {
        _setFormat("gif");
        return true;
    }

    return false;
}



void CachedImage::_setFormat(const QString format)
{
    if (format == "jpeg")
    {
        _format = JpegFormat;
        _extension = "jpg";
    }
    else if (format == "png")
    {
        _format = PngFormat;
        _extension = "png";
    }
    else if (format == "gif")
    {
        _format = GifFormat;
        _extension = "gif";
    }
    else
        qDebug() << "mime:" << format << "\nurl:" << _url;

    emit formatChanged();
}
