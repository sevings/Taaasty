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

#ifndef POSTER_H
#define POSTER_H

#include <QObject>

#include "defines.h"



class Poster : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool         loading    READ isLoading  NOTIFY loadingChanged)
    Q_PROPERTY(UploadModel* images     READ images     CONSTANT)

    Q_PROPERTY(qint64 kBytesSent  READ kBytesSent  NOTIFY kBytesSentChanged)
    Q_PROPERTY(qint64 kBytesTotal READ kBytesTotal NOTIFY kBytesTotalChanged)

public:
    enum Privacy
    {
        Voting,
        Public,
        Private
    };

    Q_ENUMS(Privacy)

    explicit Poster(QObject* parent = 0);

    Entry*          entry() const;
    bool            isLoading() const;
    UploadModel*    images();

    qint64 kBytesSent()  const { return _kBytesSent; }
    qint64 kBytesTotal() const { return _kBytesTotal; }

signals:
    void posted(EntryPtr entry);
    void edited();
    void loadingChanged();

    void kBytesSentChanged();
    void kBytesTotalChanged();

public slots:
    void postImage(QString title,                  Privacy privacy, int tlogId = 0);
    void postQuote(QString text,  QString source,  Privacy privacy, int tlogId = 0);
    void postVideo(QString title, QString url,     Privacy privacy, int tlogId = 0);
    void postText( QString title, QString content, Privacy privacy, int tlogId = 0);
    void postAnonymous(QString title, QString content);

    void putImage(int id, QString title,                  Privacy privacy);
    void putQuote(int id, QString text,  QString source,  Privacy privacy);
    void putVideo(int id, QString title, QString url,     Privacy privacy);
    void putText( int id, QString title, QString content, Privacy privacy);
    void putAnonymous(int id, QString title, QString content);

private slots:
    void _setProgress(qint64 bytes, qint64 bytesTotal);
    void _createPostedEntry(const QJsonObject& data);
    void _updateEditedEntry(const QJsonObject& data);

private:
    QString _privacyValue(const Privacy& privacy) const;

    void    _clearHtml(QString& text) const;

    void    _prepare();
    void    _prepare(int tlogId);
    void    _prepareTitle(QString& title);
    void    _prepareText(QString& content);
    void    _prepare(QString& title, int tlogId);
    void    _prepare(QString& title, QString& content);
    void    _prepare(QString& title, QString& content, int tlogId);
    void    _postPrepared();
    void    _putPrepared();

    int             _tlogId;
    EntryPtr        _entry;
    ApiRequestPtr   _request;
    UploadModelPtr  _images;

    qint64 _kBytesSent;
    qint64 _kBytesTotal;
};

#endif // POSTER_H
