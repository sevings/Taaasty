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

#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <QJsonObject>

#include "tastylistmodel.h"

class User;
class Bayes;



class UsersModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode)

public:
    enum Mode {
        FollowingsMode,
        FollowersMode,
        MyFollowingsMode,
        MyFollowersMode,
        MyIgnoredMode
    };

    Q_ENUMS(Mode)

    UsersModel(QObject* parent = nullptr);

    virtual bool hasMore() const override;

    virtual void setMode(const Mode mode) { _mode = mode; }
    Mode mode() const {return _mode; }

protected:
    QHash<int, QByteArray> roleNames() const override;

    Mode _mode;
};

#endif // USERSMODEL_H
