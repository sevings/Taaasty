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

class Author;



class Tlog: public TastyData
{
    Q_OBJECT

    Q_PROPERTY(int          tlogId              READ tlogId   WRITE setId   NOTIFY updated) //! \todo remove
    Q_PROPERTY(QString      slug                READ slug   WRITE setSlug   NOTIFY updated)
    Q_PROPERTY(QString      title               MEMBER _title               NOTIFY updated)
    Q_PROPERTY(QString      entriesCount        MEMBER _entriesCount        NOTIFY updated)
    Q_PROPERTY(QString      publicEntriesCount  MEMBER _publicEntriesCount  NOTIFY updated)
    Q_PROPERTY(QString      privateEntriesCount MEMBER _privateEntriesCount NOTIFY updated)
    Q_PROPERTY(QString      favoritesCount      MEMBER _favoritesCount      NOTIFY updated)
    Q_PROPERTY(QString      commentsCount       MEMBER _commentsCount       NOTIFY updated)
    Q_PROPERTY(QString      tagsCount           MEMBER _tagsCount           NOTIFY updated)
    Q_PROPERTY(QString      daysCount           MEMBER _daysCount           NOTIFY updated)
    Q_PROPERTY(QString      followersCount      MEMBER _followersCount      NOTIFY updated)
    Q_PROPERTY(QString      followingsCount     MEMBER _followingsCount     NOTIFY updated)
    Q_PROPERTY(QString      ignoredCount        MEMBER _ignoredCount        NOTIFY updated)
    Q_PROPERTY(Relationship myRelationship      MEMBER _myRelation          NOTIFY updated)
    Q_PROPERTY(Relationship hisRelationship     MEMBER _hisRelation         NOTIFY updated)
    Q_PROPERTY(Author*      author              READ author                 NOTIFY updated)

public:
    enum Relationship {
        Undefined,
        Me,
        Friend,
        Ignored,
        None
    };

    Q_ENUMS(Relationship)

    Tlog(QObject* parent = nullptr);
    Tlog(const QJsonObject data, QObject* parent = nullptr);

    Author* author() const { return _author; }

public slots:
    int  tlogId() const                 { return _id; }
    void setId(const int id);

    QString slug() const                { return _slug; }
    void    setSlug(const QString slug);

    void init(const QJsonObject data);
    void reload();

signals:
    void updated();
    void loadingChanged();

private:
    Relationship _relationship(const QJsonObject& data, const QString field);

    QString         _slug;
    QString         _title;
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
    Author*         _author;
};
