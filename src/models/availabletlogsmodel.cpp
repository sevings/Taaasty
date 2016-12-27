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

#include "availabletlogsmodel.h"

#include "flowsmodel.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../data/Flow.h"
#include "../data/User.h"



AvailableTlogsModel::AvailableTlogsModel(QObject* parent)
    : QAbstractProxyModel(parent)
{
    qDebug() << "AvailableTlogsModel";

    auto source = new FlowsModel(this);
    source->setMode(FlowsModel::AvailableMode);
    setSourceModel(source);

    Q_TEST(connect(source, &QAbstractItemModel::endInsertRows,
        this, &AvailableTlogsModel::flowsLoaded));
}



int AvailableTlogsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return sourceModel()->rowCount(parent) + 1;
}



QVariant AvailableTlogsModel::data(const QModelIndex& index, int role) const
{
    if (!index.row())
    {
        if (role == Qt::UserRole)
            return 0;
        if (role == Qt::UserRole + 1)
            return pTasty->me()->name();
        return QVariant();
    }

    auto flow = sourceModel()->data(index, Qt::UserRole).value<Flow*>();
    if (!flow)
        return QVariant();

    if (role == Qt::UserRole)
        return flow->id();
    if (role == Qt::UserRole + 1)
        return flow->name();
    return QVariant();
}



QModelIndex AvailableTlogsModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    return createIndex(sourceIndex.row() + 1, 0);
}



QModelIndex AvailableTlogsModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return sourceModel()->createIndex(proxyIndex.row() - 1, 0);
}



virtual QHash<int, QByteArray> AvailableTlogsModel::roleNames() const override
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole]     = "tlog";
    roles[Qt::UserRole + 1] = "name";
    return roles;
}



int AvailableTlogsModel::tlogIndex(int id) const
{
    if (id < 0)
        return -1;
    if (id == 0)
        return 0;

    auto source = static_cast<FlowsModel*>(sourceModel());
    return source->tlogIndex(id);
}
