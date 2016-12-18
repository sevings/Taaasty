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



class UsersModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(int tlog  READ tlog WRITE setTlog)
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

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    virtual bool hasMore() const override;

    virtual void setMode(const Mode mode);
    Mode mode() const {return _mode; }

    void setTlog(const int tlog);
    int tlog() const {return _tlog; }

    void downloadAll();

signals:
    void downloadCompleted();

private slots:
    void _addItems(const QJsonObject& data);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    bool         _loadAll;

    QList<User*> _users;

    Mode         _mode;
    QString      _url;
    QString      _field;

    int          _tlog;
    int          _total;
    int          _lastPosition;
};

#endif // USERSMODEL_H
