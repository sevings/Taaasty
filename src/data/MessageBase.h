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

#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <QObject>
#include <QDateTime>

#include "TastyData.h"

class User;



class MessageBase : public TastyData
{
    Q_OBJECT

    Q_PROPERTY(QString  createdAt       MEMBER _createdAt      NOTIFY baseUpdated)
    Q_PROPERTY(QString  text            MEMBER _text           NOTIFY textUpdated)
    Q_PROPERTY(QString  truncatedText   MEMBER _truncatedText  NOTIFY textUpdated)
    Q_PROPERTY(User*    user            MEMBER _user           NOTIFY userUpdated)
    Q_PROPERTY(bool     isRead          MEMBER _read           NOTIFY readChanged)
    Q_PROPERTY(bool     containsImage   MEMBER _containsImage  NOTIFY baseUpdated)

public:
    explicit MessageBase(QObject* parent = nullptr);

    bool        isRead() const;
    QDateTime   createdDate() const;

    QString text() const;

    User* user() const;

signals:
    void userUpdated();
    void baseUpdated();
    void textUpdated();
    void readChanged(bool read);

protected:
    void _setTruncatedText();
    void _setDate(const QString& d);

    QString     _createdAt;
    QDateTime   _date;
    QString     _text;
    QString     _truncatedText;
    User*       _user;
    bool        _read;
    bool        _containsImage;
};

#endif // MESSAGEBASE_H
