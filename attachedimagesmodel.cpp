#include "attachedimagesmodel.h"

#include "data/AttachedImage.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>



AttachedImagesModel::AttachedImagesModel(const QJsonArray *data, QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug() << "AttachedImagesModel";

    if (!data)
        return;

    for (int i = 0; i < data->size(); i++)
        _images << new AttachedImage(data->at(i).toObject(), this);
}



int AttachedImagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _images.size();
}



QVariant AttachedImagesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _images.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<AttachedImage*>(_images.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



AttachedImage *AttachedImagesModel::first() const
{
    return _images.isEmpty() ? nullptr : _images.first();
}



double AttachedImagesModel::listRatio() const
{
    double h = 0;
    foreach (auto img, _images)
        h += img->_height / (double)img->_width;

    return h;
}



QHash<int, QByteArray> AttachedImagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "image";
    return roles;
}
