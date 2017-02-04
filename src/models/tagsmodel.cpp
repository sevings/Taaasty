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

#include "tagsmodel.h"

#include <QJsonObject>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"



TagsModel::TagsModel(QObject* parent)
    : TastyListModel(parent)
    , _tlog(0)
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



bool TagsModel::canFetchMore(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    
    return _tlog > 0 && _names.isEmpty() && !isLoading();
}



void TagsModel::fetchMore(const QModelIndex& parent) 
{
    if (isLoading() || !canFetchMore(parent))
        return;
    
    _loadRequest = new ApiRequest(QStringLiteral("v1/tlog/%1/tags.json").arg(_tlog));
    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonArray)), this, SLOT(_setData(QJsonArray))));

    _initLoad();
}



void TagsModel::setTlog(int tlog)
{
    if (tlog <= 0 || _tlog == tlog)
        return;

    _tlog = tlog;

    beginResetModel();

    _names.clear();
    _counts.clear();

    endResetModel();
    
    fetchMore(QModelIndex());
}



QHash<int, QByteArray> TagsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole]   = "tagName";
    roles[Qt::UserRole+1] = "tagCount";
    return roles;
}



void TagsModel::_setData(const QJsonArray& data)
{
    beginResetModel();

    foreach (auto tag, data)
    {
        _names << tag.toObject().value(QStringLiteral("name")).toString();
        _counts << tag.toObject().value(QStringLiteral("taggings_count")).toInt();
    }
    
    endResetModel();
}
