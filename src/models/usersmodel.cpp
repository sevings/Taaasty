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

#include "usersmodel.h"

#include <QDebug>

#include <QVariant>

#include "../defines.h"

#include "../apirequest.h"
#include "../data/User.h"



UsersModel::UsersModel(QObject* parent)
    : TastyListModel(parent)
    , _loadAll(false)
    , _tlog(0)
    , _total(1)
    , _lastPosition(0)
{
    setMode(FollowingsMode);
}



int UsersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _users.size();
}



QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0)
        return QVariant();

    if (role != Qt::UserRole)
        return QVariant();

    if (index.row() >= _users.size())
        return QVariant();

    return QVariant::fromValue<User*>(_users.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool UsersModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _users.size() < _total;
}



void UsersModel::fetchMore(const QModelIndex& parent)
{
    if (isLoading() || !canFetchMore(parent)
            || ((_mode == FollowingsMode || _mode == FollowersMode) && !_tlog))
        return;

    QString url = _url;
    if (_lastPosition)
        url += QStringLiteral("&since_position=%1").arg(_lastPosition);

    auto opt = _optionsForFetchMore(_mode == MyFollowersMode
                                    || _mode == MyFollowingsMode || _mode == MyIgnoredMode);
    _loadRequest = new ApiRequest(url, opt);

    connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));

    _initLoad();
}



bool UsersModel::hasMore() const
{
    return canFetchMore(QModelIndex());
}



void UsersModel::setMode(const UsersModel::Mode mode)
{
    _mode = mode;

    switch(mode)
    {
    case FollowersMode:
        _url = QStringLiteral("v1/tlog/%1/followers.json?limit=20").arg(_tlog);
        _field = "reader";
        break;
    case FollowingsMode:
        _url = QStringLiteral("v1/tlog/%1/followings.json?limit=20").arg(_tlog);
        _field = "user";
        break;
    case MyFollowingsMode:
        _url = QStringLiteral("v1/relationships/to/friend.json?limit=20");
        _field = "user";
        break;
    case MyFollowersMode:
        _url = QStringLiteral("v1/relationships/by/friend.json?limit=20"); //! \todo test me
        _field = "reader";
        break;
    case MyIgnoredMode:
        _url = QStringLiteral("v1/relationships/to/ignored.json?limit=20");
        _field = "user";
        break;
    default:
        qDebug() << "users mode =" << mode;
    }
}



void UsersModel::setTlog(const int tlog)
{
    if (tlog <= 0)
        return;

    beginResetModel();

    _tlog = tlog;
    _total = 1;
    _lastPosition = 0;

    delete _loadRequest;

    setMode(_mode);

    qDeleteAll(_users);
    _users.clear();

    emit rowCountChanged();

    endResetModel();

    emit hasMoreChanged();
}



void UsersModel::downloadAll()
{
    _loadAll = true;
    fetchMore(QModelIndex());
}



void UsersModel::_addItems(const QJsonObject& data)
{
    _total = data.value(QStringLiteral("total_count")).toInt();

    auto list = data.value(QStringLiteral("relationships")).toArray();
    if (list.isEmpty())
    {
        _total = _users.size();
        emit hasMoreChanged();

        if (_loadAll)
        {
            _loadAll = false;
            emit downloadCompleted();
        }

        return;
    }

    _lastPosition = list.last().toObject().value(QStringLiteral("position")).toInt();

    QList<User*> users;
    users.reserve(list.size());
    foreach(auto item, list)
    {
        auto userData = item.toObject().value(_field).toObject();
        auto user = new User(userData, this);
        users << user;
    }

    beginInsertRows(QModelIndex(), _users.size(), _users.size() + list.size() - 1);
    _users << users;
    endInsertRows();

    emit rowCountChanged();

    if (_users.size() >= _total)
        emit hasMoreChanged();

    _loadRequest = nullptr;

    if (!_loadAll)
    {
        emit loadingChanged();
        return;
    }

    if (_users.size() < _total)
        fetchMore(QModelIndex());
    else
    {
        _loadAll = false;
        emit downloadCompleted();

        emit loadingChanged();
    }
}



QHash<int, QByteArray> UsersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "user";
    return roles;
}
