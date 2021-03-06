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

#ifndef UPLOADMODEL_H
#define UPLOADMODEL_H

#include <QStringListModel>
#include <QFutureWatcher>
#include <QImageReader>
#include <QHttpPart>

#include "../defines.h"

#ifdef Q_OS_ANDROID
class AndroidImagePicker;
#endif



class UploadModel : public QStringListModel
{
    Q_OBJECT

    Q_PROPERTY(int size READ rowCount NOTIFY rowCountChanged)

#ifdef Q_OS_ANDROID
    friend class AndroidImagePicker;
#endif

public:
    UploadModel();
    ~UploadModel();

    void setName(const QString& name);

    bool isLoading() const;

    const QList<QHttpPart>& parts() const;

    void loadFiles();

signals:
    void loaded();
    void rowCountChanged();

public slots:
    void append();
    void remove(int i);

    void load();
    void save();
    void clear();

private slots:
    void _append(const QString& fileName);

private:
    void _loadFiles(bool optimize);

    QList<QHttpPart>             _parts;
    QMap<QString, QImageReader*> _readers;
    QFutureWatcher<void>         _watcher;
    bool                         _savable;
    QString                      _name;

#ifdef Q_OS_ANDROID
    AndroidImagePicker*          _picker;
#endif
};

#endif // UPLOADMODEL_H
