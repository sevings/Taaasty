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

#ifndef CACHEDIMAGE_H
#define CACHEDIMAGE_H

#include <QNetworkReply>
#include <QSslError>
#include <QFutureWatcher>
#include <QPixmap>

class CacheManager;


class CachedImage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUrl         source              READ source                         NOTIFY available)
    Q_PROPERTY(int          kbytesReceived      READ received                       NOTIFY receivedChanged)
    Q_PROPERTY(int          kbytesTotal         READ total                          NOTIFY totalChanged)
    Q_PROPERTY(bool         isDownloading       READ isDownloading                  NOTIFY downloadingChanged)
    Q_PROPERTY(bool         available           READ isAvailable                    NOTIFY available)
    Q_PROPERTY(QString      extension           READ extension   WRITE setExtension NOTIFY extensionChanged)
    Q_PROPERTY(ImageFormat  format              READ format                         NOTIFY extensionChanged)
    Q_PROPERTY(QString      fileName            READ fileName                       CONSTANT)

public:
    enum ImageFormat { UnknownFormat, GifFormat, JpegFormat, PngFormat };
    Q_ENUMS(ImageFormat)

    explicit CachedImage(CacheManager* parent = nullptr, const QString& url = QString());

    void    loadFile();

    QPixmap pixmap();

    QUrl    source() const;
    QString sourceFileName() const;

    int     received() const    { return _kbytesReceived; }
    int     total() const       { return _kbytesTotal; }

    bool    isDownloading() const;
    bool    isAvailable() const;

    QString extension() const   { return _extension; }
    void    setExtension(QString format);

    QString fileName() const;
    QString url() const         { return _url; }

    ImageFormat format() const;

signals:
    void available();
    void receivedChanged();
    void totalChanged();
    void downloadingChanged();
    void extensionChanged();

    void savingError();
    void fileSaved();

public slots:
    void getInfo();
    void download();
    void abortDownload();
    void saveToFile(const QString& filename);

private slots:
    void _setProperties();
    void _saveData();
    void _changeBytes(qint64 bytesReceived, qint64 bytesTotal);
    void _printError(QNetworkReply::NetworkError code);
    void _printErrors(const QList<QSslError>& errors);
    void _readPixmap(const QPixmap& pm);

private:
    QString _filePath() const;
    QString _path() const;
    void    _saveFile(QByteArray* data);
    QPixmap _loadFile();

    CacheManager*        _man;       //-V122
    QNetworkReply*       _headReply; //-V122
    QNetworkReply*       _reply;     //-V122
    QFutureWatcher<void> _watcher;
    QPixmap              _pixmap;

    ImageFormat _format;

    uint    _hash;
    QString _url;
    QString _extension;

    int     _kbytesReceived;
    int     _kbytesTotal;
    bool    _autoload;
    bool    _available;
    bool    _loaded;
};

#endif // CACHEDIMAGE_H
