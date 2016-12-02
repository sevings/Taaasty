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

#include "flowsmodel.h"

#include <QDebug>
#include <QJsonArray>

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../data/Flow.h"



FlowsModel::FlowsModel(QObject* parent)
    : TastyListModel(parent)
    , _page(1)
{
    qDebug() << "FlowsModel";

    setMode(Tasty::instance()->isAuthorized() ? MyMode : PopularMode);
    
    Q_TEST(connect(Tasty::instance(), &Tasty::authorizedChanged, this, &FlowsModel::_resetIfMy));
}



int FlowsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _flows.size();
}



QVariant FlowsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _flows.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Flow*>(_flows.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool FlowsModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _hasMore;
}



void FlowsModel::fetchMore(const QModelIndex& parent)
{
    if (!_hasMore || isLoading() || parent.isValid())
        return;

    qDebug() << "FlowsModel::fetchMore";

    auto url = _url.arg(_page++);
    auto opt = _optionsForFetchMore(_mode == MyMode || _mode == AvailableMode);
    _loadRequest = new ApiRequest(url, opt);

    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject))));

    _initLoad();
}



void FlowsModel::setMode(const FlowsModel::Mode mode)
{
    beginResetModel();

    _mode = mode;

    switch(_mode)
    {
    case PopularMode:
        _url = "v1/flows.json?sort=popular&page=%1&limit=20";
        break;
    case NewestMode:
        _url = "v1/flows.json?sort=newest&page=%1&limit=20";
        break;
    case MyMode:
        _url = "v1/flows.json?sort=my&page=%1&limit=20";
        break;
    case AvailableMode:
        _url = "v1/flows/available.json&page=%1&limit=20";
        break;
    default:
        qDebug() << "Flows mode:" << _mode;
    }

    qDeleteAll(_flows);
    _flows.clear();

    _page = 1;
    _hasMore = true;
    emit hasMoreChanged();

    delete _loadRequest;

    endResetModel();
}



void FlowsModel::reset()
{
    beginResetModel();

    qDeleteAll(_flows);
    _flows.clear();

    _page = 1;
    _hasMore = true;
    emit hasMoreChanged();

    delete _loadRequest;

    endResetModel();
}



QHash<int, QByteArray> FlowsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "flow";
    return roles;
}



void FlowsModel::_addItems(QJsonObject data)
{
    qDebug() << "FlowsModel::_addItems";

    auto hasMore = data.value("has_more").toBool();
    if (hasMore != _hasMore)
    {
        _hasMore = hasMore;
        emit hasMoreChanged();
    }

    auto flows = data.value("items").toArray();
    if (flows.isEmpty())
        return;

    beginInsertRows(QModelIndex(), _flows.size(), _flows.size() + flows.size() - 1);

    foreach (auto flowData, flows)
    {
        auto flow = new Flow(this);
        flow->init(flowData.toObject().value("flow").toObject());
        _flows << flow;
    }

    endInsertRows();
}



void FlowsModel::_resetIfMy()
{
    if (_mode != MyMode && _mode != AvailableMode)
        return;

    if (Tasty::instance()->isAuthorized())
        reset();
    else
        setMode(PopularMode);
}
