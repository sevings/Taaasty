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



Poster::Poster(QObject* parent) : QObject(parent)
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



QString Poster::errorString() const
{
    return _errorString;
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

    QString data;
    QTextStream(&data) << "title=" << title
                       << "&text=" << content
                       << "&privacy=" << privacyValue;

    if (tlogId > 0)
        data += QString("&tlog_id=%1").arg(tlogId);

    qDebug() << data;

    _request = new ApiRequest("v1/entries/text.json", ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_createPostedEntry(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(error(int,QString)),         this, SLOT(_setErrorString(int,QString))));
    Q_TEST(connect(_request, &QObject::destroyed,                this, &Poster::loadingChanged, Qt::QueuedConnection));

    emit loadingChanged();
}



void Poster::postAnonymous(QString title, QString content)
{
    _tlogId = -1;

    _prepareText(title, content);

    QString data;
    QTextStream(&data) << "title=" << title
                       << "&text=" << content;

    qDebug() << data;

    _request = new ApiRequest("v1/entries/anonymous.json", ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(_request, SIGNAL(success(const QJsonObject)), this, SLOT(_createPostedEntry(const QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(error(int,QString)),         this, SLOT(_setErrorString(int,QString))));
    Q_TEST(connect(_request, &QObject::destroyed,                this, &Poster::loadingChanged, Qt::QueuedConnection));

    emit loadingChanged();
}



void Poster::_createPostedEntry(QJsonObject data)
{
    _entry = EntryPtr::create((QObject*)nullptr);
    _entry->init(data);

    emit posted(_entry);

    emit Tasty::instance()->entryCreated(_entry->id(), _tlogId);
}



void Poster::_setErrorString(int errorCode, QString str)
{
    qDebug() << "Poster error code" << errorCode;

    _errorString = str;
    emit errorStringChanged();
}



void Poster::_prepareText(QString& title, QString& content) const
{
    if (title.isEmpty())
        title = "&nbsp;";

    //! \todo: insert paragraphs

    title = QUrl::toPercentEncoding(title);
    content = QUrl::toPercentEncoding(content);
}
