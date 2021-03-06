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

#include "TastyData.h"
#include "Conversation.h"

class Author;
class Flow;



class Tlog: public TastyData
{
    Q_OBJECT

    Q_PROPERTY(int              tlogId              READ tlogId   WRITE setId   NOTIFY updated) //! \todo remove
    Q_PROPERTY(QString          slug                READ slug   WRITE setSlug   NOTIFY updated)
    Q_PROPERTY(QString          title               MEMBER _title               NOTIFY updated)
    Q_PROPERTY(QString          entriesCount        MEMBER _entriesCount        NOTIFY updated)
    Q_PROPERTY(QString          publicEntriesCount  MEMBER _publicEntriesCount  NOTIFY updated)
    Q_PROPERTY(QString          privateEntriesCount MEMBER _privateEntriesCount NOTIFY updated)
    Q_PROPERTY(QString          favoritesCount      MEMBER _favoritesCount      NOTIFY updated)
    Q_PROPERTY(QString          commentsCount       MEMBER _commentsCount       NOTIFY updated)
    Q_PROPERTY(QString          tagsCount           MEMBER _tagsCount           NOTIFY updated)
    Q_PROPERTY(QString          daysCount           MEMBER _daysCount           NOTIFY updated)
    Q_PROPERTY(QString          followersCount      MEMBER _followersCount      NOTIFY updated)
    Q_PROPERTY(QString          followingsCount     MEMBER _followingsCount     NOTIFY updated)
    Q_PROPERTY(QString          ignoredCount        MEMBER _ignoredCount        NOTIFY updated)
    Q_PROPERTY(Relationship     myRelationship      MEMBER _myRelation          NOTIFY myRelationChanged)
    Q_PROPERTY(Relationship     hisRelationship     MEMBER _hisRelation         NOTIFY hisRelationChanged)
    Q_PROPERTY(Author*          author              READ author                 NOTIFY updated)
    Q_PROPERTY(Flow*            flow                READ flow                   NOTIFY updated)
    Q_PROPERTY(Conversation*    chat                READ chat                   CONSTANT)
    Q_PROPERTY(bool             changingRelation    READ changingRelation       NOTIFY changingRelationChanged)

public:
    enum Relationship {
        Undefined,
        Me,
        Friend,
        Requested,
        Ignored,
        None
    };

    Q_ENUMS(Relationship)

    Tlog(QObject* parent = nullptr);
    Tlog(const QJsonObject& data, QObject* parent = nullptr);

    Author* author() const { return _author; }

    Flow* flow() const { return _flow; }

    Conversation* chat();

    bool changingRelation() const;

    int  tlogId() const                 { return _id; }
    void setId(const int id);

    QString slug() const                { return _slug; }
    void    setSlug(const QString& slug);

    QString tag() const                 { return _tag; }

public slots:
    void init(const QJsonObject& data);
    void reload();

    void follow();
    void unfollow();

    void ignore();
    void cancelIgnoring();

    void unsubscribeHim();

    void approveFriendRequest();
    void disapproveFriendRequest();

signals:
    void updated();
    void myRelationChanged();
    void hisRelationChanged();
    void changingRelationChanged();

private slots:
    void _setMyRelation(const QJsonObject& data);
    void _setHisRelation(const QJsonObject& data);

private:
    void _changeMyRelation(const QString& url);
    void _handleFriendRequest(const QString& url);

    Relationship _relationship(const QJsonObject& data, const QString& field);

    QString         _slug;
    QString         _title;
    QString         _tag;
    QString         _entriesCount;
    QString         _publicEntriesCount;
    QString         _privateEntriesCount;
    QString         _favoritesCount;
    QString         _commentsCount;
    QString         _tagsCount;
    QString         _daysCount;
    QString         _followersCount;
    QString         _followingsCount;
    QString         _ignoredCount;
    Relationship    _myRelation;
    Relationship    _hisRelation;
    Author*         _author; //-V122
    Flow*           _flow;   //-V122
    ChatPtr         _chat;

    QPointer<ApiRequest> _relationRequest;
};
