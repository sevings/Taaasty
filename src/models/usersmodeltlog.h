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

#ifndef USERSMODELTLOG_H
#define USERSMODELTLOG_H

#include "usersmodel.h"



class UsersModelTlog : public UsersModel
{
    Q_OBJECT

    friend class UsersModelBayes;

    Q_PROPERTY(int tlog READ tlog WRITE setTlog)

public:
    UsersModelTlog(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE virtual void setMode(const Mode mode) override;

    Q_INVOKABLE void setTlog(const int tlog);
    Q_INVOKABLE int tlog() const {return _tlog; }

    void downloadAll();

signals:
    void downloadCompleted();

private slots:
    void _addItems(QJsonObject data);

private:
    bool _loadAll;

    QList<User*> _users;
    QString _url;
    QString _field;
    int  _tlog;
    int  _total;
    int  _lastPosition;
};

#endif // USERSMODELTLOG_H
