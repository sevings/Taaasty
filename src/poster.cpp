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

#include "models/uploadmodel.h"

#include "data/Entry.h"
#include "tasty.h"
#include "apirequest.h"



Poster::Poster(QObject* parent)
    : QObject(parent)
    , _tlogId(0)
    , _images(nullptr)
    , _kBytesSent(0)
    , _kBytesTotal(0)
{

}



Entry* Poster::entry() const
{
    return _entry.data();
}



bool Poster::isLoading() const
{
    return _request && _request->isRunning();
}



UploadModel* Poster::images()
{
    if (!_images)
    {
        _images = new UploadModel(this);
        _images->load();
    }
    
    return _images;
}



void Poster::postImage(QString title, Poster::Privacy privacy, int tlogId)
{
    Q_ASSERT(tlogId >= 0);
    
    if (!_images || !_images->rowCount())
    {
        emit pTasty->error(0, QStringLiteral("Добавьте хотя бы одно изображение"));
        return;
    }
    
    _prepare(title, tlogId);

    _request->setUrl(QStringLiteral("v1/entries/image.json"));
    _request->addFormData("title", title);
    _request->addFormData("privacy", _privacyValue(privacy));
    _request->addImages(_images);

    _postPrepared();    
}



void Poster::postQuote(QString text, QString source, Privacy privacy, int tlogId)
{
    Q_ASSERT(tlogId >= 0);
    
    _prepare(tlogId);

    _request->setUrl(QStringLiteral("v1/entries/quote.json"));
    _request->addFormData("text", text.trimmed());
    _request->addFormData("source", source.trimmed());
    _request->addFormData("privacy", _privacyValue(privacy));
    
    _postPrepared();        
}



void Poster::postVideo(QString title, QString url, Privacy privacy, int tlogId)
{
    Q_ASSERT(tlogId >= 0);
    
    _prepare(title, tlogId);

    _request->setUrl(QStringLiteral("v1/entries/video.json"));
    _request->addFormData("titlle", title);
    _request->addFormData("video_url", url);
    _request->addFormData("privacy", _privacyValue(privacy));
    
    _postPrepared();      
}



void Poster::postText(QString title, QString content, Poster::Privacy privacy, int tlogId)
{
    if (tlogId < 0)
    {
        postAnonymous(title, content);
        return;
    }

    _prepare(title, content, tlogId);

    _request->setUrl(QStringLiteral("v1/entries/text.json"));
    _request->addFormData("title", title);
    _request->addFormData("text", content);
    _request->addFormData("privacy", _privacyValue(privacy));
    
    _postPrepared();
}



void Poster::postAnonymous(QString title, QString content)
{
    _prepare(title, content, -1);

    _request->setUrl(QStringLiteral("v1/entries/anonymous.json"));
    _request->addFormData("title", title);
    _request->addFormData("text", content);

    _postPrepared();
}



void Poster::_setProgress(qint64 bytes, qint64 bytesTotal)
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



void Poster::_createPostedEntry(const QJsonObject& data)
{
    _entry = EntryPtr::create(nullptr);
    _entry->init(data);

    emit posted(_entry);

    emit pTasty->entryCreated(_entry->id(), _tlogId);
}



QString Poster::_privacyValue(const Privacy& privacy) const
{
    switch (privacy) 
    {
    case Voting:
        return "public_with_voting";
    case Public:
        return "public";
    case Private:
        return "private";
    default:
        return QString();
    }    
}



void Poster::_prepare(int tlogId)
{
    _tlogId = tlogId;
    
    if (!_request)
        _request = new ApiRequest(ApiRequest::AllOptions);
    
    if (tlogId > 0)
        _request->addFormData("tlog_id", tlogId);
}



void Poster::_prepare(QString& title, int tlogId)
{
    _prepare(tlogId);
    
    if (title.isEmpty())
        title = "&nbsp;";
}



void Poster::_prepare(QString& title, QString& content, int tlogId)
{
    _prepare(title, tlogId);

    auto ps = content.split('\n');
    for (auto it = ps.begin(); it != ps.end(); ++it)
        if (it->isEmpty())
            *it = QStringLiteral("<br>");
        else
            it->prepend(QStringLiteral("<p>")).append(QStringLiteral("</p>"));

    content = ps.join(QString());
}



void Poster::_postPrepared()
{
    _request->post();

    Q_TEST(connect(_request, &ApiRequest::progress,              this, &Poster::_setProgress));
    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_createPostedEntry(const QJsonObject))));
    Q_TEST(connect(_request, &QObject::destroyed,                this, &Poster::loadingChanged, Qt::QueuedConnection));

    emit loadingChanged();    
}
