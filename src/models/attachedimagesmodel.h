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

#ifndef ATTACHEDIMAGESMODEL_H
#define ATTACHEDIMAGESMODEL_H

#include <QAbstractListModel>

class QJsonArray;
class AttachedImage;



class AttachedImagesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(AttachedImage* first READ first CONSTANT)

public:
    AttachedImagesModel(QObject* parent = nullptr);
    AttachedImagesModel(const QJsonArray& data, QObject* parent = nullptr);

    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void clear();

    AttachedImage* first() const;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<AttachedImage*> _images;
};

#endif // ATTACHEDIMAGESMODEL_H
