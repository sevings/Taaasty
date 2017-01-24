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

#include "cachedimage.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QImageReader>
#include <QBuffer>
#include <QtConcurrent>
#include <QStandardPaths>
#include <QQmlEngine>
#include <QtMath>

#include <QDebug>

#include "../defines.h"

#include "cachemanager.h"
#include "../tasty.h"



CachedImage::CachedImage(CacheManager* parent, const QString& url)
    : QObject()
    , _man(parent)
    , _headReply(nullptr)
    , _reply(nullptr)
    , _format(UnknownFormat)
    , _url(url)
    , _kbytesReceived(0)
    , _kbytesTotal(0)
    , _fileSize(0)
    , _available(false)
{
    if (!_init())
        return;

    if (_man->autoload(_kbytesTotal))
        download();
    else
        getInfo();
}



CachedImage::CachedImage(CacheManager* parent, const QString& url,
                         const QString& format, int size)
    : QObject()
    , _man(parent)
    , _headReply(nullptr)
    , _reply(nullptr)
    , _format(UnknownFormat)
    , _url(url)
    , _kbytesReceived(0)
    , _kbytesTotal(0)
    , _fileSize(size)
    , _available(false)
{
    if (!_init())
        return;

    setExtension(format);

    loadFile();
}



CachedImage::~CachedImage()
{

}



void CachedImage::loadFile()
{
    if (_watcher.isRunning() || isAvailable())
        return;

    auto future = QtConcurrent::run(this, &CachedImage::_loadFile);
    _watcher.setFuture(future);
}



void CachedImage::removeFile()
{
    QFile::remove(_filePath());

    _fileSize = 0;
    emit fileSizeChanged();
}



QPixmap CachedImage::pixmap()
{
    Q_ASSERT(_format != GifFormat);

    QPixmap pm(1, 1);
    if (!QPixmapCache::find(_pmKey, &pm))
    {
        _available = false;
        emit availableChanged();

        loadFile();
    }

    return pm;
}



QUrl CachedImage::source() const
{
    return QUrl::fromLocalFile(_filePath());
}



QString CachedImage::sourceFileName() const
{
    return QString("%1.%2").arg(_fileName).arg(_extension);
}



int CachedImage::diskSpace() const
{
    return qCeil(_fileSize / 4096.0) * 4096;
}



bool CachedImage::isDownloading() const
{
    return (_reply && _reply->isRunning()) || _watcher.isRunning();
}



bool CachedImage::isAvailable() const
{
    return _available;
}



void CachedImage::setExtension(QString format)
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
    else if (format.isEmpty() && !_url.isEmpty())
    {
        auto ext = _url.split(".").last();
        if (!ext.isEmpty() && ext.size() != _url.size())
            setExtension(ext);
        return;
    }
    else
        qDebug() << "mime:" << format << "\nurl:" << _url;

    emit extensionChanged();
}



QString CachedImage::fileName() const
{
    if (_url.isEmpty())
        return _url;

    auto full = _url.split("/").last().split(".");
    if (full.isEmpty())
        return QString();

    if (full.size() == 1)
        return full.first();

    return full.at(full.size() - 2);
}




void CachedImage::getInfo()
{
    if (_headReply || _reply || _watcher.isRunning() || isAvailable())
        return;

    qDebug() << "Getting image info from" << _url;

    _headReply = _man->web()->head(QNetworkRequest(_url));

    Q_TEST(connect(_headReply, SIGNAL(finished()),                         this, SLOT(_setProperties())));
    Q_TEST(connect(_headReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_printError(QNetworkReply::NetworkError))));
    Q_TEST(connect(_headReply, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(_printErrors(QList<QSslError>))));
}



void CachedImage::download()
{
    if (_url.isEmpty() || isDownloading())
        return;

    if (isAvailable())
        return;

    if (_headReply)
        _headReply->abort();

    qDebug() << "Downloading image from" << _url;

    _reply = _man->web()->get(QNetworkRequest(_url));

    Q_TEST(connect(_reply, SIGNAL(finished()),                         this, SLOT(_saveData())));
    Q_TEST(connect(_reply, SIGNAL(downloadProgress(qint64,qint64)),    this, SLOT(_changeBytes(qint64,qint64))));
    Q_TEST(connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_printError(QNetworkReply::NetworkError))));
    Q_TEST(connect(_reply, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(_printErrors(QList<QSslError>))));

    emit downloadingChanged();
}



void CachedImage::abortDownload()
{
    if (!_reply || !_reply->isRunning())
        return;

    _reply->abort();

    emit downloadingChanged();

    _kbytesReceived = 0;
    emit receivedChanged();
}



void CachedImage::saveToFile(const QString& filename)
{
    if (!isAvailable())
        return;

    if (filename.isEmpty())
        return;

    auto pathTo = QString("%1/Taaasty/")
            .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    QDir().mkpath(pathTo);

    auto to = QString("%1%2%3%4")
            .arg(pathTo)
            .arg(filename)
            .arg(_extension.isEmpty() ? QString() : ".")
            .arg(_extension);

    if (QFile::exists(to))// && !QFile::remove(to))
    {
        qDebug() << "File exists" << to;
        emit pTasty->error(0, QString("Файл уже существует:\n%1").arg(to));
        emit savingError();
        return;
    }

    if (!QFile::copy(_filePath(), to))
    {
        qDebug() << "Error copying" << to;
        emit pTasty->error(0, QString("Не удалось сохранить изображение в %1").arg(to));
        emit savingError();
        return;
    }

    emit pTasty->info(QString("Изображение сохранено в %1").arg(to));
    emit fileSaved();
}



void CachedImage::_setProperties()
{
    if (!_headReply)
        return;

    if (_headReply->error() != QNetworkReply::NoError)
    {
        _headReply->deleteLater();
        _headReply = nullptr;

        return;
    }

    auto mime = _headReply->header(QNetworkRequest::ContentTypeHeader).toString().split('/');
    if (mime.isEmpty())
        mime << "";

    if (!mime.first().isEmpty() && mime.first().toLower() != "image")
        qDebug() << "mime:" << mime.first() << "\nurl:" << _url;

    setExtension(mime.last());

    auto length = _headReply->header(QNetworkRequest::ContentLengthHeader).toInt();
    _kbytesTotal = length / 1024;

    emit totalChanged();

    _headReply->deleteLater();
    _headReply = nullptr;

    if (_man->autoload(_kbytesTotal))
        download();
}



void CachedImage::_saveData()
{
    Q_ASSERT(_reply);
    if (!_reply)
        return;

    if (_reply->error() != QNetworkReply::NoError)
    {
        _reply->deleteLater();
        _reply = nullptr;

        emit downloadingChanged();

        return;
    }

    auto data = new QByteArray;
    *data = _reply->readAll();

    QBuffer buffer(data);
    buffer.open(QIODevice::ReadOnly);
    QImageReader reader(&buffer);
    auto format = reader.format();
    setExtension(QString::fromLatin1(format));

    auto future = QtConcurrent::run(this, &CachedImage::_saveFile, data);
    _watcher.setFuture(future);

    _reply->deleteLater();
    _reply = nullptr;
}



void CachedImage::_changeBytes(qint64 bytesReceived, qint64 bytesTotal)
{
    if (!_reply || _reply->error() != QNetworkReply::NoError)
        return;

    _kbytesReceived = bytesReceived / 1024;
    emit receivedChanged();

    _kbytesTotal = bytesTotal / 1024;
    emit totalChanged();
}



void CachedImage::_printError(QNetworkReply::NetworkError code)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply && code != QNetworkReply::OperationCanceledError)
        qDebug() << "image web error" << code << reply->errorString()
                 << "\nhttp status: " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                 << "\ndata: " << reply->readAll();
}



void CachedImage::_printErrors(const QList<QSslError>& errors)
{
    foreach(auto e, errors)
        qDebug() << e.errorString();

    auto reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT(reply);
    if (reply)
        reply->ignoreSslErrors();
}



void CachedImage::_readPixmap(const QPixmap& pm)
{
    if (pm.isNull())
    {
        if (_man->autoload(_kbytesTotal))
            download();
        else if (!_kbytesTotal)
            getInfo();

        if (_available)
        {
            _available = false;
            emit availableChanged();
        }
    }
    else
    {
        if (_format != GifFormat)
            _pmKey = QPixmapCache::insert(pm);

        if (!_available)
        {
            _available = true;
            emit availableChanged();
        }
    }
}



bool CachedImage::_init()
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (!_man || _url.isEmpty())
        return false;

    Q_TEST(connect(&_watcher, &QFutureWatcher<void>::finished,
                   this, &CachedImage::downloadingChanged, Qt::UniqueConnection));

    if (_url.startsWith("//"))
        _url.prepend("http:");

    _fileName.setNum(qHash(_url), 16);
    _folder = _fileName.at(0);

    return true;
}



QString CachedImage::_filePath() const
{
    return QString("%1%2").arg(_path()).arg(sourceFileName());
}



QString CachedImage::_path() const
{
    return QString("%1/%2/").arg(_man->path()).arg(_folder);
}



void CachedImage::_saveFile(QByteArray* data)
{
    QPixmap pic;
    pic.loadFromData(*data);
    if (_man->maxWidth() && _format != UnknownFormat && _format != GifFormat
            && pic.width() > _man->maxWidth())
    {
        pic = pic.scaledToWidth(_man->maxWidth(), Qt::SmoothTransformation);
        QBuffer buffer(data);
        buffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
        pic.save(&buffer, _extension.toLatin1().data());
    }

    _fileSize = data->size();
    Q_TEST(QMetaObject::invokeMethod(this, "fileSizeChanged", Qt::QueuedConnection));

    Q_TEST(QMetaObject::invokeMethod(this, "_readPixmap", Qt::QueuedConnection, Q_ARG(QPixmap, pic)));

    QDir().mkpath(_path());

    const auto filePath = _filePath();
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(*data);
        file.close();
    }
    else
        qDebug() << "Cannot open file for writing:" << filePath;

    delete data;
}



void CachedImage::_loadFile()
{
    Q_ASSERT(_format != UnknownFormat);

    QPixmap pixmap;
    if (_format != GifFormat)
    {
        const auto path = _filePath();
        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            auto data = file.readAll();
            auto format = _extension.toLatin1().constData();
            pixmap.loadFromData(data, format);
        }
    }
    else
        pixmap = QPixmap(1, 1);

    Q_TEST(QMetaObject::invokeMethod(this, "_readPixmap", Qt::QueuedConnection, Q_ARG(QPixmap, pixmap)));
}
