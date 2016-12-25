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

#include "uploadmodel.h"

#include <QFileInfo>
#include <QtConcurrent>
#include <QUrl>

#ifdef Q_OS_ANDROID

#else
#   include <QFileDialog>
#   include <QStandardPaths>
#endif

#include "../tasty.h"
#include "../settings.h"

#define TASTY_IMAGE_WIDTH 712



UploadModel::UploadModel(QObject* parent)
    : QStringListModel(parent)
    , _savable(false)
{
    Q_TEST(connect(&_watcher, &QFutureWatcher<void>::finished, this, &UploadModel::loaded));
}



UploadModel::~UploadModel()
{
    if (_savable)
        save();

    qDeleteAll(_readers);
}



bool UploadModel::isLoading() const
{
    return _watcher.isRunning();
}



const QList<QHttpPart>& UploadModel::parts() const
{
    Q_ASSERT(!isLoading());

    return _parts;
}



void UploadModel::loadFiles()
{
    if (!rowCount())
    {
        emit loaded();
        return;
    }

    auto opt = pTasty->settings()->lastOptimizeImages();
    auto future = QtConcurrent::run(this, &UploadModel::_loadFiles, opt);
    _watcher.setFuture(future);
}



void UploadModel::append()
{
#ifdef Q_OS_ANDROID

#else
    auto path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    auto fileName = QFileDialog::getOpenFileName(nullptr, tr("Open Image"), path, tr("Image Files (*.png *.jpg *.bmp *.gif)"));
    _append(fileName);
#endif
}



void UploadModel::remove(int i)
{
    if (isLoading())
        return;

    auto fileName = QUrl(data(index(i), Qt::DisplayRole).toString()).toLocalFile();
    delete _readers.take(fileName);

    removeRow(i);
}



void UploadModel::load()
{
    _savable = true;

    auto images = pTasty->settings()->lastImages();
    foreach (auto image, images)
        _append(image);
}



void UploadModel::save()
{
    pTasty->settings()->setLastImages(_readers.keys());
}



void UploadModel::clear()
{
    if (isLoading())
        return;

    _parts.clear();

    qDeleteAll(_readers);
    _readers.clear();

    setStringList(QStringList());

    if (_savable)
        save();
}



void UploadModel::_append(const QString& fileName)
{
    if (fileName.isEmpty())
        return;

    if (isLoading())
        return;

    QScopedPointer<QImageReader> reader(new QImageReader(fileName));
    if (!reader->canRead())
        return;

    if (!insertRow(rowCount()))
        return;

    if (!setData(index(rowCount() - 1), QUrl::fromLocalFile(fileName), Qt::DisplayRole))
        return;

    _readers.insert(fileName, reader.take());

    if (_savable)
        save();
}



void UploadModel::_loadFiles(bool optimize)
{
    for (auto it = _readers.cbegin(); it != _readers.cend(); ++it)
    {
        auto fileName = QFileInfo(it.key()).fileName();
        auto reader = it.value();
        auto format = reader->format();

        QHttpPart imagePart;
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
                            QString("image/").append(QString::fromLatin1(format)));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant(QString("form-data; name=\"files[]\"; filename=\"%1\"").arg(fileName)));

        if (!optimize || format == "gif" || reader->size().width() <= TASTY_IMAGE_WIDTH)
            imagePart.setBodyDevice(reader->device());
        else
        {
            auto quality = reader->quality();
            auto image = reader->read();
            image = image.scaledToWidth(TASTY_IMAGE_WIDTH, Qt::SmoothTransformation);

            QByteArray body;
            QBuffer buffer(&body);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, format.constData(), quality);
            imagePart.setBody(body);
        }

        _parts << imagePart;
    }
}