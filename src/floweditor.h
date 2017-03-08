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

#pragma once

#include <QObject>

#include "defines.h"

class Flow;



class FlowEditor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool    loading     READ isLoading   NOTIFY loadingChanged)
    Q_PROPERTY(QString pic         READ pic         NOTIFY picChanged)

    Q_PROPERTY(qint64  kBytesSent  READ kBytesSent  NOTIFY kBytesSentChanged)
    Q_PROPERTY(qint64  kBytesTotal READ kBytesTotal NOTIFY kBytesTotalChanged)

public:
    explicit FlowEditor(QObject* parent = 0);

    Q_INVOKABLE void setFlow(Flow* flow);

    QString pic()         const;
    bool    isLoading()   const;
    qint64  kBytesSent()  const { return _kBytesSent; }
    qint64  kBytesTotal() const { return _kBytesTotal; }

signals:
    void created(const QJsonObject& data);
    void edited(const QJsonObject& data);

    void loadingChanged();
    void kBytesSentChanged();
    void kBytesTotalChanged();

    void picChanged();

public slots:
    void create(const QString& name, const QString& title);
    void update(int id, const QString& name, const QString& title,
        const QString& slug, bool privacy, bool premoderate);

    void changePic();
    void clearPic();

private slots:
    void _setProgress(qint64 bytes, qint64 bytesTotal);

private:
    UploadModelPtr  _images;
    ApiRequestPtr   _request;
    qint64          _kBytesSent;
    qint64          _kBytesTotal;
    QString         _picUrl;
};
