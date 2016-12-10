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

#include "attachedimagesmodel.h"

#include "../data/AttachedImage.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>



AttachedImagesModel::AttachedImagesModel(QObject* parent)
    : QAbstractListModel(parent)
{

}



AttachedImagesModel::AttachedImagesModel(const QJsonArray* data, QObject* parent)
    : QAbstractListModel(parent)
{
//    qDebug() << "AttachedImagesModel";

    if (!data)
        return;

    for (int i = 0; i < data->size(); i++)
        _images << new AttachedImage(data->at(i).toObject(), this);
}



int AttachedImagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _images.size();
}



QVariant AttachedImagesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _images.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<AttachedImage*>(_images.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



AttachedImage *AttachedImagesModel::first() const
{
    return _images.isEmpty() ? nullptr : _images.first();
}



double AttachedImagesModel::listRatio() const
{
    double h = 0;
    foreach (auto img, _images)
        h += img->_height / (double)img->_width; //-V2005

    return h;
}



QHash<int, QByteArray> AttachedImagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "image";
    return roles;
}
