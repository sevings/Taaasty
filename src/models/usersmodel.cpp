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

#include "usersmodel.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "../defines.h"



UsersModel::UsersModel(QObject* parent)
    : TastyListModel(parent)
{
    setMode(FollowingsMode);
}



bool UsersModel::hasMore() const
{
    return canFetchMore(QModelIndex());
}



QHash<int, QByteArray> UsersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "user";
    return roles;
}
