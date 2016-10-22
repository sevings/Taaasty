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

#include "tagsmodel.h"

#include <QJsonObject>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"



TagsModel::TagsModel(QObject* parent)
    : TastyListModel(parent)
{
    qDebug() << "TagsModel";
}



int TagsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    Q_ASSERT(_names.size() == _counts.size());
    
    return _names.size();
}



QVariant TagsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _names.size())
        return QVariant();

    if (role == Qt::UserRole)
        return _names.at(index.row());
    if (role == Qt::UserRole + 1)
        return _counts.at(index.row());

    qDebug() << "role" << role;

    return QVariant();
}



void TagsModel::setTlog(const int tlog)
{
    if (tlog <= 0)
        return;

    _loadRequest = new ApiRequest(QString("v1/tlog/%1/tags.json").arg(tlog));
    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonArray)), this, SLOT(_setData(QJsonArray))));

    _initLoad();

    beginResetModel();

    _names.clear();
    _counts.clear();

    endResetModel();
}



QHash<int, QByteArray> TagsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole]   = "tagName";
    roles[Qt::UserRole+1] = "tagCount";
    return roles;
}



void TagsModel::_setData(QJsonArray data)
{
    beginResetModel();

    foreach (auto tag, data)
    {
        _names << tag.toObject().value("name").toString();
        _counts << tag.toObject().value("taggings_count").toInt();
    }
    
    endResetModel();
}
