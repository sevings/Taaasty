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
#include <QImage>
#include <QBuffer>
#include <QtConcurrent>
#include <QStandardPaths>

#include <QDebug>

#include "../defines.h"

#include "cachemanager.h"
#include "../tasty.h"



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
    Q_ASSERT(_man);

    Q_TEST(connect(&_saveWatcher, &QFutureWatcher<void>::finished, this, &CachedImage::downloadingChanged, Qt::QueuedConnection));
    Q_TEST(connect(&_saveWatcher, &QFutureWatcher<void>::finished, this, [&]()
    {
        _available = true;
        emit available();
    }, Qt::QueuedConnection));
    
    if (!_man || _url.isEmpty())
        return;

    if (_url.startsWith("//"))
        _url = "http:" + _url;

    _hash = qHash(_url);
    if (_exists())
    {
        _available = true;
        emit available();
        return;
    }

    qDebug() << "CachedImage from url: " << url;

    if (_man->autoload())
        download();
    else
        getInfo();
}



QUrl CachedImage::source() const
{
    return QUrl::fromLocalFile(_path());
}



QString CachedImage::sourceFileName() const
{
    return QString("%1.%2").arg(_hash).arg(_extension);
}



bool CachedImage::isDownloading() const
{
    return (_reply && _reply->isRunning()) || _saveWatcher.isRunning();
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
    if (_headReply || _reply)
        return;

    _headReply = _man->web()->head(QNetworkRequest(_url));

    Q_TEST(connect(_headReply, SIGNAL(finished()),                         this, SLOT(_setProperties())));
    Q_TEST(connect(_headReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(_printError(QNetworkReply::NetworkError))));
    Q_TEST(connect(_headReply, SIGNAL(sslErrors(QList<QSslError>)),        this, SLOT(_printErrors(QList<QSslError>))));
}



void CachedImage::download()
{
    if (_url.isEmpty() || isDownloading())
        return;

    if (_available)
    {
        emit available();
        return;
    }

    if (_headReply)
        _headReply->abort();

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



void CachedImage::saveToFile(const QString filename)
{
    if (!_available)
        return;

    if (filename.isEmpty())
        return;

    auto pathTo = QString("%1/Taaasty/")
            .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    QDir dirTo;
    dirTo.mkpath(pathTo);

    auto to = QString("%1%2%3%4")
            .arg(pathTo)
            .arg(filename)
            .arg(_extension.isEmpty() ? QString() : ".")
            .arg(_extension);

    if (QFile::exists(to))// && !QFile::remove(to))
    {
        qDebug() << "File exists" << to;
        emit Tasty::instance()->error(0, QString("Файл уже существует:\n%1").arg(to));
        emit savingError();
        return;
    }

    if (!QFile::copy(_path(), to))
    {
        qDebug() << "Error copying" << to;
        emit Tasty::instance()->error(0, QString("Не удалось сохранить изображение в %1").arg(to));
        emit savingError();
        return;
    }

    emit Tasty::instance()->info(QString("Изображение сохранено в %1").arg(to));
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

    if (_reply->error() != QNetworkReply::NoError )
    {
        _reply->deleteLater();
        _reply = nullptr;

        emit downloadingChanged();

        return;
    }

    auto data = new QByteArray;
    *data = _reply->readAll();

    if (data->startsWith((char)0x89))      //-V2005
        setExtension("png");
    else if (data->startsWith((char)0xFF)) //-V2005
        setExtension("jpeg");
    else if (data->startsWith((char)0x47)) //-V2005
        setExtension("gif");

    auto future = QtConcurrent::run(this, &CachedImage::_saveFile, data);
    _saveWatcher.setFuture(future);

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



CachedImage::ImageFormat CachedImage::format() const
{
    return _format;
}



bool CachedImage::_exists()
{
    const auto path = QString("%1/%2.%3").arg(_man->path()).arg(_hash);

    if (QFile::exists(path.arg("jpg")))
    {
        setExtension("jpeg");
        return true;
    }
    if (QFile::exists(path.arg("png")))
    {
        setExtension("png");
        return true;
    }
    if (QFile::exists(path.arg("gif")))
    {
        setExtension("gif");
        return true;
    }

    return false;
}



QString CachedImage::_path() const
{
    return QString("%1/%2").arg(_man->path()).arg(sourceFileName());
}



void CachedImage::_saveFile(QByteArray* data) const
{
    auto path = _path();

    qDebug() << "Saving image from" << _url << "to" << path;

    if (_man->maxWidth() && _format != UnknownFormat && _format != GifFormat)
    {
        auto pic = QImage::fromData(*data);
        if (pic.width() > _man->maxWidth())
        {
            pic = pic.scaledToWidth(_man->maxWidth(), Qt::SmoothTransformation);
            QBuffer buffer(data);
            buffer.open(QIODevice::WriteOnly | QIODevice::Truncate);
            pic.save(&buffer, _extension.toLatin1().data());
        }
    }

    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(*data);
        file.close();
    }
    else
        qDebug() << "Cannot open file for writing:" << path;

    delete data;
}
