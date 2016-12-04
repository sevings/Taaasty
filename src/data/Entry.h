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
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QEnableSharedFromThis>

#include "../defines.h"
#include "TastyData.h"

class Author;
class Tlog;
class Rating;
class Media;
class CommentsModel;
class AttachedImagesModel;
class MessagesModel;



class EntryBase: public TastyData
{
    Q_OBJECT

public:
    EntryBase(QObject* parent = nullptr);

    void load(int id);

    Author* author() const;
    QString text() const;
    QString title() const;
    QString type() const;

public slots:
    int entryId() const { return _id; }

signals:
    void loaded();
    void loadingError();

protected slots:
    void _initBase(QJsonObject data);
    void _maybeError();

protected:
    Author*     _author;
    QString     _text;
    QString     _title;
    QString     _type;
};



class Entry: public EntryBase, public QEnableSharedFromThis<Entry>
{
    Q_OBJECT

    Q_PROPERTY(int         entryId        READ entryId WRITE setId  NOTIFY updated) //! \todo remove 
    Q_PROPERTY(QString     createdAt      MEMBER _createdAt         NOTIFY updated)
    Q_PROPERTY(QString     url            MEMBER _url               NOTIFY updated)
    Q_PROPERTY(QString     type           MEMBER _type              NOTIFY updated) //! \todo enum
    Q_PROPERTY(bool        isVotable      READ   isVotable          NOTIFY updated)
    Q_PROPERTY(bool        isWatchable    MEMBER _isWatchable       NOTIFY updated)
    Q_PROPERTY(bool        isWatched      MEMBER _isWatched         NOTIFY watchedChanged)
    Q_PROPERTY(bool        isFavoritable  MEMBER _isFavoritable     NOTIFY updated)
    Q_PROPERTY(bool        isFavorited    MEMBER _isFavorited       NOTIFY favoritedChanged)
    Q_PROPERTY(bool        isPrivate      MEMBER _isPrivate         NOTIFY updated)
    Q_PROPERTY(bool        isFixed        READ   isFixed            NOTIFY updated)
    Q_PROPERTY(bool        isDeletable    MEMBER _isDeletable       NOTIFY updated)
    Q_PROPERTY(bool        isEditable     MEMBER _isEditable        NOTIFY updated)
    Q_PROPERTY(Tlog*       tlog           READ tlog                 NOTIFY updated)
    Q_PROPERTY(Author*     author         MEMBER _author            NOTIFY updated)
    Q_PROPERTY(Rating*     rating         MEMBER _rating            NOTIFY updated)
    Q_PROPERTY(int         commentsCount  MEMBER _commentsCount     NOTIFY commentsCountChanged)
    Q_PROPERTY(QString     title          MEMBER _title             NOTIFY htmlUpdated)
    Q_PROPERTY(QString     truncatedTitle MEMBER _truncatedTitle    NOTIFY updated)
    Q_PROPERTY(QString     text           MEMBER _text              NOTIFY htmlUpdated)
    Q_PROPERTY(QString     truncatedText  MEMBER _truncatedText     NOTIFY updated)
    Q_PROPERTY(QString     source         MEMBER _source            NOTIFY updated)
    Q_PROPERTY(Media*      media          MEMBER _media             NOTIFY updated)
//    Q_PROPERTY(QJsonObject imagePreview   MEMBER _imagePreview      NOTIFY updated)
    Q_PROPERTY(int         wordCount      MEMBER _wordCount         NOTIFY updated)

    Q_PROPERTY(Conversation*  chat        READ chat                 NOTIFY updated)
    Q_PROPERTY(CommentsModel* comments    READ commentsModel        NOTIFY updated)
    Q_PROPERTY(AttachedImagesModel* attachedImagesModel READ attachedImagesModel NOTIFY updated)

public:
    Entry();
    Entry(Conversation* chat);
    ~Entry();

    Q_INVOKABLE CommentsModel*       commentsModel();
    Q_INVOKABLE AttachedImagesModel* attachedImagesModel() { return _attachedImagesModel; }

    int wordCount() const;

    Rating* rating() const;

    Tlog* tlog() const;

    int commentsCount() const;

    Conversation* chat();

    bool      isVotable() const;
    bool      isFixed() const;
    QDateTime fixedAt() const;
    int       chatId() const;

public slots:
    void init(const QJsonObject data);

    void setId(const int id);
    void reload();

    void addComment(const QString text);
    void watch();
    void favorite();

    void deleteEntry();

signals:
    void updated();
    void updatingError();
    void htmlUpdated();

    void commentsCountChanged();
    void watchedChanged();
    void favoritedChanged();
    void commentAdded(const QJsonObject data);
    void addingCommentError();

    void entryDeleted();

private slots:
    void _changeWatched(const QJsonObject data);
    void _changeFavorited(const QJsonObject data);
    void _setCommentsCount(int tc);
    void _setWatched();
    void _correctHtml();
    void _setChatId();
    void _deleteEntry(QJsonObject data);

private:
    QString     _createdAt;
    QString     _url;
    bool        _isVotable;
    bool        _isWatchable;
    bool        _isWatched;
    bool        _isFavoritable;
    bool        _isFavorited;
    bool        _isPrivate;
    bool        _isFixed;
    QDateTime   _fixedAt;
    bool        _isDeletable;
    bool        _isEditable;
    Tlog*       _tlog;
    Rating*     _rating;
    int         _commentsCount;
    QString     _truncatedTitle;
    QString     _truncatedText;
    QString     _source;
    Media*      _media;
//    QJsonObject _imagePreview;
    int         _wordCount;
    int         _chatId;

    QJsonArray _commentsData;

    CommentsModel*       _commentsModel;
    AttachedImagesModel* _attachedImagesModel;
    ChatPtr              _chat;

    QPointer<ApiRequest> _entryRequest;
};
