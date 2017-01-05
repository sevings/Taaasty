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

#include "reposter.h"

#include "models/feedmodel.h"

#include "data/Entry.h"
#include "data/Tlog.h"
#include "data/Flow.h"

#include "tasty.h"
#include "tastydatacache.h"
#include "settings.h"
#include "apirequest.h"



Reposter::Reposter(FeedModel* parent)
    : QObject(parent)
    , _model(parent)
    , _entryId(0)
{

}



bool Reposter::isRepostable(int entryId) const
{
    if (!_model)
        return false;

    if (_model->contains(entryId))
        return false;

    auto entry = pTasty->dataCache()->entry(entryId);
    if (!entry)
        return false;

    if (entry->tlog()->id() == _model->tlogId())
        return false;

    int myId = pTasty->settings()->userId();

    if (_model->mode() == FeedModel::MyTlogMode
            && myId == entry->tlog()->id())
        return false;

    if (_model->mode() != FeedModel::TlogMode)
        return false;

    if (!_model->tlog() || !_model->tlog()->flow()
        || !_model->tlog()->flow()->isWritable())
        return false;

    if (myId == _model->tlogId())
        return false;

    return true;
}



bool Reposter::isUnrepostable(int entryId) const
{
    if (!_model)
        return false;

    if (!_model->contains(entryId))
        return false;

    auto entry = pTasty->dataCache()->entry(entryId);
    if (!entry)
        return false;

    if (entry->tlog()->id() == _model->tlogId())
        return false;

    int myId = pTasty->settings()->userId();

    if ((_model->mode() == FeedModel::MyTlogMode
         || _model->tlogId() == myId)
            && entry->tlog()->id() != myId)
        return true;

    if (_model->mode() != FeedModel::TlogMode)
        return false;

    if (!_model->tlog() || !_model->tlog()->flow()
        || !_model->tlog()->flow()->isWritable())
        return false;

    if (myId == _model->tlogId())
        return false;

    return true;
}



void Reposter::repost(int entryId)
{
    if (_request || !isRepostable(entryId) || !_model)
        return;

    _entryId = entryId;

    int tlog = _tlogId();
    auto url = QString("v1/reposts.json");

    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->addFormData("tlog_id", tlog);
    _request->addFormData("entry_id", entryId);
    _request->post();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_addRepost(QJsonObject))));
}



void Reposter::unrepost(int entryId)
{
    if (_request || !isUnrepostable(entryId) || !_model)
        return;

    _entryId = entryId;

    int tlog = _tlogId();
    auto url = QString("v1/reposts.json?tlog_id=%1&entry_id=%2").arg(tlog).arg(entryId);

    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->deleteResource();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_removeRepost(QJsonObject))));
}



int Reposter::_tlogId() const
{
    return _model->mode() == FeedModel::MyTlogMode
                ? pTasty->settings()->userId() : _model->tlogId();
}



void Reposter::_addRepost(const QJsonObject& data)
{
    emit pTasty->info("Репост добавлен");

    int id = data.value("id").toInt();
    auto entry = pTasty->dataCache()->entry(id);
    if (!entry)
        entry = EntryPtr::create(nullptr);

    entry->init(data);

    emit reposted(entry);
}



void Reposter::_removeRepost(const QJsonObject& data)
{
    if (data.value("status").toString() == "success")
    {
        emit pTasty->info("Репост удален");
        emit unreposted(_entryId);
    }
    else
    {
        emit pTasty->error(0, "При удалении репоста произошла ошибка");
        qDebug() << data;
    }
}
