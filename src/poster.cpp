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

#include "poster.h"

#include <QUrl>

#include "data/Entry.h"
#include "tasty.h"
#include "apirequest.h"



Poster::Poster(QObject* parent)
    : QObject(parent)
    , _tlogId(0)
{

}



Entry* Poster::entry() const
{
    return _entry.data();
}



bool Poster::isLoading() const
{
    return _request;
}



void Poster::postText(QString title, QString content, Poster::Privacy privacy, int tlogId)
{
    if (tlogId < 0)
    {
        postAnonymous(title, content);
        return;
    }

    _tlogId = tlogId;

    _prepareText(title, content);

    QString privacyValue;
    switch (privacy) {
    case Voting:
        privacyValue = "public_with_voting";
        break;
    case Public:
        privacyValue = "public";
        break;
    case Private:
        privacyValue = "private";
        break;
    }

    _request = new ApiRequest("v1/entries/text.json", ApiRequest::AllOptions);
    _request->addFormData("title", title);
    _request->addFormData("text", content);
    _request->addFormData("privacy", privacyValue);
    if (tlogId > 0)
        _request->addFormData("tlog_id", tlogId);
    _request->post();

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_createPostedEntry(QJsonObject))));
    Q_TEST(connect(_request, &QObject::destroyed,                this, &Poster::loadingChanged, Qt::QueuedConnection));
   
    emit loadingChanged();
}



void Poster::postAnonymous(QString title, QString content)
{
    _tlogId = -1;

    _prepareText(title, content);

    _request = new ApiRequest("v1/entries/anonymous.json", ApiRequest::AllOptions);
    _request->addFormData("title", title);
    _request->addFormData("text", content);
    _request->post();

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_createPostedEntry(const QJsonObject))));
    Q_TEST(connect(_request, &QObject::destroyed,                this, &Poster::loadingChanged, Qt::QueuedConnection));

    emit loadingChanged();
}



void Poster::_createPostedEntry(const QJsonObject& data)
{
    _entry = EntryPtr::create(nullptr);
    _entry->init(data);

    emit posted(_entry);

    emit pTasty->entryCreated(_entry->id(), _tlogId);
}



void Poster::_prepareText(QString& title, QString& content) const
{
    if (title.isEmpty())
        title = "&nbsp;";

    auto ps = content.split('\n');
    for (auto it = ps.begin(); it != ps.end(); ++it)
        if (it->isEmpty())
            *it = QString("</br>");
        else
            it->prepend("<p>").append("</p>");

    content = ps.join(QString());
}
