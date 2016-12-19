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

#ifndef TAGSMODEL_H
#define TAGSMODEL_H

#include <QJsonArray>

#include "tastylistmodel.h"



class TagsModel : public TastyListModel
{
    Q_OBJECT

public:
    TagsModel(QObject* parent = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE void setTlog(int tlog);

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _setData(const QJsonArray& data);

private:
    QList<QString>  _names;
    QList<int>      _counts;
    int             _tlog;
};

#endif // TAGSMODEL_H
