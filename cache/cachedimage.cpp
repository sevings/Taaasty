#include "cachedimage.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QImage>
#include <QBuffer>
#include <QtConcurrent>

#include <QDebug>

#include "../defines.h"

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
    , _available(false)
{
    if (!_man || _url.isEmpty())
        return;

    _hash = QString::number(qHash(_url));
    if (_exists())
    {
        _available = true;
        emit available();
        return;
    }

    if (_man->autoload())
        download();
    else
        getInfo();
}



QString CachedImage::source() const
{
    auto path = QString("%1/%2.%3").arg(_man->path()).arg(_hash).arg(_extension);
    return QUrl::fromLocalFile(path).toString();
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

    _headReply = _man->web()->head(QNetworkRequest(_url));

    Q_TEST(connect(_headReply, SIGNAL(finished()),                         this, SLOT(_setProperties())));
    Q_TEST(connect(_headReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_printError(QNetworkReply::NetworkError))));
    Q_TEST(connect(_headReply, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(_printErrors(QList<QSslError>))));
}



void CachedImage::download()
{
    if (_url.isEmpty() || !isReadyToDownload() || isDownloading())
        return;

    if (_available)
    {
        emit available();
        return;
    }

    _reply = _man->web()->get(QNetworkRequest(_url));

    Q_TEST(connect(_reply, SIGNAL(finished()),                         this, SLOT(_saveData())));
    Q_TEST(connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_printError(QNetworkReply::NetworkError))));
    Q_TEST(connect(_reply, SIGNAL(downloadProgress(qint64,qint64)),    this, SLOT(_changeBytes(qint64,qint64))));
    Q_TEST(connect(_reply, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(_printErrors(QList<QSslError>))));

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

    _kbytesReceived = 0;
    emit receivedChanged();
}



void CachedImage::_setProperties()
{
    auto mime = _headReply->header(QNetworkRequest::ContentTypeHeader).toString().split('/');
    if (mime.isEmpty())
        mime << "";
    if (!mime.first().isEmpty() && mime.first().toLower() != "image")
    {
        qDebug() << "mime:" << mime.first() << "\nurl:" << _url;
        return;
    }

    _setFormat(mime.last());

    auto length = _headReply->header(QNetworkRequest::ContentLengthHeader).toInt();
    _kbytesTotal = length / 1024;

    emit totalChanged();

    _headReply->deleteLater();
    _headReply = nullptr;

    emit readyToDownload();
}



void CachedImage::_saveData()
{
    if (_reply->error() != QNetworkReply::NoError ) {
//        if (_reply->error() == QNetworkReply::OperationCanceledError || _reply->error() == QNetworkReply::)
//            return;

        _reply->deleteLater();
        _reply = nullptr;

//        download();
        return;
    }

    _data = _reply->readAll();
    if (_format == UnknownFormat)
    {
        if (_data.startsWith(0x89))
            _setFormat("png");
        else if (_data.startsWith(0xFF))
            _setFormat("jpeg");
        else if (_data.startsWith(0x47))
            _setFormat("gif");
    }

    _reply->deleteLater();
    _reply = nullptr;

    QtConcurrent::run(this, &CachedImage::_saveFile);
}



void CachedImage::_changeBytes(qint64 bytesReceived, qint64 bytesTotal)
{
    _kbytesReceived = bytesReceived / 1024;
    emit receivedChanged();

    _kbytesTotal = bytesTotal / 1024;
    emit totalChanged();
}



void CachedImage::_printError(QNetworkReply::NetworkError code)
{
    auto reply = _headReply ? _headReply : _reply;
    if (code != QNetworkReply::OperationCanceledError)
        qDebug() << "image web error" << code << reply->errorString()
                 << "\nhttp status: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                 << "\ndata: " << reply->readAll();
}



void CachedImage::_printErrors(const QList<QSslError>& errors)
{
    foreach(auto e, errors)
        qDebug() << e.errorString();

    (_headReply ? _headReply : _reply)->ignoreSslErrors();
}



bool CachedImage::_exists()
{
    const auto path = QString("%1/%2.%3").arg(_man->path()).arg(_hash);

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



void CachedImage::_setFormat(QString format)
{
    format = format.toLower();

    if (format == "jpeg" || format == "jpg")
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
    else if (format.isEmpty())
    {
        auto ext = _url.split(".").last();
        if (!ext.isEmpty())
            _setFormat(ext);
        return;
    }
    else
        qDebug() << "mime:" << format << "\nurl:" << _url;

    emit formatChanged();
}



void CachedImage::_saveFile()
{
    auto path = QString("%1/%2.%3").arg(_man->path()).arg(_hash).arg(_extension);
    if (_man->maxWidth() && _format != UnknownFormat && _format != GifFormat)
    {
        auto pic = QImage::fromData(_data);
        if (pic.width() > _man->maxWidth())
        {
            pic = pic.scaledToWidth(_man->maxWidth(), Qt::SmoothTransformation);
            QBuffer buffer(&_data);
            buffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
            pic.save(&buffer, _extension.toLatin1().data());
        }
    }

    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(_data);
    file.close();

    _data.clear();

    _available = true;
    emit available();
}