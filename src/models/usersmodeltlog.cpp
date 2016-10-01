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

#include "usersmodeltlog.h"

#include <QDebug>

#include <QVariant>

#include "../defines.h"

#include "../apirequest.h"
#include "../data/User.h"



UsersModelTlog::UsersModelTlog(QObject* parent)
    : UsersModel(parent)
    , _loadAll(false)
    , _tlog(0)
    , _total(1)
    , _lastPosition(0)
{
    setMode(FollowingsMode);
}



int UsersModelTlog::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _users.size();
}



QVariant UsersModelTlog::data(const QModelIndex &index, int role) const
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



bool UsersModelTlog::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _users.size() < _total;
}



void UsersModelTlog::fetchMore(const QModelIndex& parent)
{
    if (_loading || !canFetchMore(parent)
            || ((_mode == FollowingsMode || _mode == FollowersMode) && !_tlog))
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url;
    if (_lastPosition)
        url += QString("&since_position=%1").arg(_lastPosition);

    auto accessTokenRequired = _mode == MyFollowersMode || _mode == MyFollowingsMode || _mode == MyIgnoredMode;
    auto request = new ApiRequest(url, accessTokenRequired);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));
}



void UsersModelTlog::setMode(const UsersModel::Mode mode)
{
    _mode = mode;

    switch(mode)
    {
    case FollowersMode:
        _url = QString("v2/tlog/%1/followers.json?limit=20").arg(_tlog);
        _field = "reader";
        break;
    case FollowingsMode:
        _url = QString("v2/tlog/%1/followings.json?limit=20").arg(_tlog);
        _field = "user";
        break;
    case MyFollowingsMode:
        _url = QString("v1/relationships/to/friend.json?limit=20");
        _field = "user";
        break;
    case MyFollowersMode:
        _url = QString("v1/relationships/by/friend.json?limit=20"); //! TODO: test me
        _field = "reader";
        break;
    case MyIgnoredMode:
        _url = QString("v1/relationships/to/ignored.json?limit=20");
        _field = "user";
        break;
    default:
        qDebug() << "users mode =" << mode;
    }
}



void UsersModelTlog::setTlog(const int tlog)
{
    if (tlog <= 0)
        return;

    beginResetModel();

    _tlog = tlog;
    _total = 1;
    _lastPosition = 0;

    _loading = false;
    emit loadingChanged();

    setMode(_mode);

    qDeleteAll(_users);

    endResetModel();

    emit hasMoreChanged();
}



void UsersModelTlog::downloadAll()
{
    _loadAll = true;
    fetchMore(QModelIndex());
}



void UsersModelTlog::_addItems(QJsonObject data)
{
    _total = data.value("total_count").toInt();

    auto list = data.value("relationships").toArray();
    if (list.isEmpty())
    {
        _total = _users.size();
        emit hasMoreChanged();

        if (_loadAll)
        {
            _loadAll = false;
            emit downloadCompleted();
        }

        _loading = false;
        emit loadingChanged();

        return;
    }

    _lastPosition = list.last().toObject().value("position").toInt();

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

    if (_users.size() >= _total)
        emit hasMoreChanged();

    _loading = false;

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
