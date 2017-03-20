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

#include "User.h"



class Author: public User
{
    Q_OBJECT

    Q_PROPERTY(bool    isFemale            MEMBER _isFemale            NOTIFY authorUpdated)
    Q_PROPERTY(bool    isPrivacy           MEMBER _isPrivacy           NOTIFY authorUpdated)
    Q_PROPERTY(bool    isOnline            MEMBER _isOnline            NOTIFY statusUpdated)
    Q_PROPERTY(bool    isFlow              MEMBER _isFlow              NOTIFY authorUpdated)
    Q_PROPERTY(bool    isPremium           MEMBER _isPremium           NOTIFY authorUpdated)
    Q_PROPERTY(bool    isDaylog            MEMBER _isDaylog            NOTIFY authorUpdated)
    Q_PROPERTY(QString title               MEMBER _title               NOTIFY authorUpdated)
    Q_PROPERTY(QString entriesCount        MEMBER _entriesCount        NOTIFY authorUpdated)
    Q_PROPERTY(QString publicEntriesCount  MEMBER _publicEntriesCount  NOTIFY authorUpdated)
    Q_PROPERTY(QString privateEntriesCount MEMBER _privateEntriesCount NOTIFY authorUpdated)
    Q_PROPERTY(QString daysCount           MEMBER _daysCount           NOTIFY authorUpdated)
    Q_PROPERTY(QString followingsCount     MEMBER _followingsCount     NOTIFY authorUpdated)
    Q_PROPERTY(QString lastSeenAt          MEMBER _lastSeenAt          NOTIFY statusUpdated)

    friend class StatusChecker;

public:
    Author(QObject* parent = nullptr);
    Author(const QJsonObject& data, QObject* parent = nullptr);

    bool isFemale() const;
    bool isFlow() const;
    bool isPremium() const;
    bool isDaylog() const;

public slots:
    void init(const QJsonObject& data);
    void checkStatus();
    void reload();

signals:
    void authorUpdated();
    void statusUpdated();

private slots:
    void _initFromTlog(const QJsonObject& data);
    void _initStatus(const QJsonArray& data);
    void _initStatus(const QJsonObject& data);

private:
    bool    _isFemale;
    bool    _isPrivacy;
    bool    _isOnline;
    bool    _isFlow;
    bool    _isPremium;
    bool    _isDaylog;
    QString _title;
    QString _entriesCount;
    QString _publicEntriesCount;
    QString _privateEntriesCount;
    QString _daysCount;
    QString _followingsCount;
    QString _lastSeenAt;
};
