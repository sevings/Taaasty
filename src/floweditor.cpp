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

#include "floweditpr.h"

#include <QUrl>

#include "models/uploadmodel.h"

#include "tasty.h"
#include "apirequest.h"
#include "settings.h"



FlowEditor::FlowEditor(QObject* parent)
    : QObject(parent)
    , _images(new UploadModel(this))
    , _kBytesSent(0)
    , _kBytesTotal(0)
{
    _images->setName(QStringLiteral("flowpic"));

    _lastOpt = pTasty->settings()->lastOptimizeImages();
    pTasty->settings()->setLastOptimizeImages(false);
}



FlowEditor::~FlowEditor()
{
    pTasty->settings()->setLastOptimizeImages(_lastOpt);
}



QString FlowEditor::pic() const
{
    return _images->data(UploadModel::index(0), Qt::DisplayRole).toString();
}



bool FlowEditor::isLoading() const
{
    return _request && _request->isRunning();
}



void FlowEditor::create(const QString& name, const QString& title)
{
    if (!_images || !_images->rowCount())
    {
        emit pTasty->error(0, QStringLiteral("Установите фон потока"));
        return;
    }

    _request = new ApiRequest(ApiRequest::AllOptions);
    _request->setUrl(QStringLiteral("v1/flows.json"));
    _request->addFormData("name", name);
    _request->addFormData("title", title);
    _request->addImages(_images);
    _request->post();

    Q_TEST(connect(_request, &ApiRequest::progress, this, &FlowEditor::_setProgress));
    Q_TEST(connect(_request, &QObject::destroyed,   this, &FlowEditor::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SIGNAL(created(const QJsonObject))));

    emit loadingChanged();
}



void FlowEditor::update(int id, const QString& name, const QString& title,
    const QString& slug, bool privacy, bool premoderate)
{
    if (!_images || !_images->rowCount())
    {
        emit pTasty->error(0, QStringLiteral("Установите фон потока"));
        return;
    }

#define STR(x) ((x) ? QStringLiteral("true") : QStringLiteral("false"))

    _request = new ApiRequest(ApiRequest::AllOptions);
    _request->setUrl(QStringLiteral("v1/flows/%1.json").arg(id));
    _request->addFormData("name", name);
    _request->addFormData("title", title);
    _request->addFormData("slug", slug);
    _request->addFormData("is_privacy",     STR(privacy));
    _request->addFormData("is_premoderate", STR(premoderate));
    _request->addImages(_images);
    _request->put();

    Q_TEST(connect(_request, &ApiRequest::progress, this, &FlowEditor::_setProgress));
    Q_TEST(connect(_request, &QObject::destroyed,   this, &FlowEditor::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SIGNAL(edited(const QJsonObject))));

    emit loadingChanged();
}



void FlowEditor::changePic()
{
    _images->remove(0);
    _images->append();

    emit picChanged();
}



void FlowEditor::_setProgress(qint64 bytes, qint64 bytesTotal)
{
    auto sent = bytes / 1024;
    if (sent != _kBytesSent)
    {
        _kBytesSent = sent;
        emit kBytesSentChanged();
    }

    auto total = bytesTotal / 1024;
    if (total != _kBytesTotal)
    {
        _kBytesTotal = total;
        emit kBytesTotalChanged();
    }
}
