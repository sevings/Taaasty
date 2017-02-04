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

#include "Comment.h"

#include "../defines.h"

#include "Notification.h"
#include "User.h"

#include "../tasty.h"
#include "../tastydatacache.h"
#include "../apirequest.h"



Comment::Comment(QObject* parent)
    : MessageBase(parent)
    , _isEditable(false)
    , _isReportable(false)
    , _isDeletable(false)
{
    _user = new User(this);
}



Comment::Comment(const QJsonObject& data, QObject *parent)
    : MessageBase(parent)
{
    _user = nullptr;

    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Comment::~Comment()
{
    pTasty->dataCache()->removeComment(_id);
}



void Comment::edit(const QString& text)
{
    if (isLoading())
        return;
    
    auto url  = QStringLiteral("v1/comments/%1.json").arg(_id);
    _request  = new ApiRequest(url, ApiRequest::AllOptions);
    _request->addFormData("text", text);
    _request->put();

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_init(const QJsonObject))));
    
    _initRequest();
}



void Comment::remove()
{
    if (isLoading())
        return;
    
    auto url = QStringLiteral("v1/comments/%1.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->deleteResource();

    Q_TEST(connect(_request, SIGNAL(success(const QString)), this, SLOT(_remove(const QString))));
    
    _initRequest();
}



void Comment::_init(const QJsonObject& data)
{
    _id             = data.value(QStringLiteral("id")).toInt();

    delete _user;
    _user           = new User(data.value(QStringLiteral("user")).toObject(), this);

    _text           = data.value(QStringLiteral("comment_html")).toString();
    auto d = data.value(QStringLiteral("created_at")).toString();
    _createdAt      = Tasty::parseDate(d);
    _setDate(d);
    _isEditable     = data.value(QStringLiteral("can_edit")).toBool();
    _isReportable   = data.value(QStringLiteral("can_report")).toBool();
    _isDeletable    = data.value(QStringLiteral("can_delete")).toBool();

    _correctHtml();
    _setTruncatedText();

    emit idChanged();
    emit userUpdated();
    emit baseUpdated();
    emit updated();

   pTasty->dataCache()->addComment(this);
}



void Comment::_update(const QJsonObject& data)
{
    auto list = data.value(QStringLiteral("comments")).toArray();
    if (list.isEmpty())
        return;

    _init(list.first().toObject());
}



void Comment::_correctHtml()
{
    Tasty::correctHtml(_text, false);

    emit textUpdated();
}



void Comment::_remove(const QString& data)
{
    if (data == "true")
        deleteLater();
    else
        qDebug() << "removing commment:" << data;
}
