#include "notificationsmodel.h"

#include <QDebug>

#include "apirequest.h"
#include "datastructures.h"



NotificationsModel::NotificationsModel(QObject* parent)
    : QAbstractListModel(parent)
    , _url("messenger/notifications.json?limit=20")
    , _loading(false)
    , _totalCount(1)
{

}



NotificationsModel::~NotificationsModel()
{
    qDeleteAll(_notifs);
}



int NotificationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _notifs.size();
}



QVariant NotificationsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _notifs.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Notification*>(_notifs.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool NotificationsModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _notifs.size() < _totalCount;
}



void NotificationsModel::fetchMore(const QModelIndex& parent)
{
//    qDebug() << "fetch more";

    if (_loading || parent.isValid() || _notifs.size() >= _totalCount)
        return;

    _loading = true;

    QString url = _url;
    if (!_notifs.isEmpty())
    {
        auto lastId = _notifs.last()->_id;
        url += QString("&to_notification_id=%1").arg(lastId);
    }
    
    auto request = new ApiRequest(url, true);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));
}



void NotificationsModel::markAsRead()
{
    if (_notifs.isEmpty())
        return;
    
    QString url = "messenger/notifications/read.json";
    QString data = QString("last_id=%1").arg(_notifs.first()->_id);
    
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_readSuccess(QJsonObject)));
}



void NotificationsModel::check()
{
    if (_loading)
        return;

    _loading = true;

    QString url = _url;
    if (!_notifs.isEmpty())
    {
        auto lastId = _notifs.last()->_id;
        url += QString("&from_notification_id=%1").arg(lastId);
    }
    
    auto request = new ApiRequest(url, true);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addNewest(QJsonObject)));
}



void NotificationsModel::_readSuccess(QJsonObject data)
{
    qDebug() << data;
}



QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "notification";
    return roles;
}



void NotificationsModel::_addItems(QJsonObject data)
{
//    qDebug() << "adding entries";

    int size = _notifs.size();
    auto list = data.value("notifications").toArray();
    if (list.isEmpty())
    {
        _totalCount = size;
        emit hasMoreChanged();
        _loading = false;
        return;
    }

    _totalCount = data.value("total_count").toInt();
    
    beginInsertRows(QModelIndex(), size, size + list.size() - 1);

    _notifs.reserve(size + list.size());
    foreach(auto notif, list)
    {
        auto notification = new Notification(notif.toObject(), this);
        _notifs.insert(size, notification);
    }

    endInsertRows();

    if (_notifs.size() >= _totalCount)
        emit hasMoreChanged();

    _loading = false;
}



void NotificationsModel::_addNewest(QJsonObject data)
{
//    qDebug() << "adding entries";

    auto list = data.value("notifications").toArray();
    if (list.isEmpty())
    {
        _loading = false;
        return;
    }
    
    _totalCount = data.value("total_count").toInt();

    beginInsertRows(QModelIndex(), 0, list.size() - 1);

    foreach(auto notif, list)
    {
        auto notification = new Notification(notif.toObject(), this);
        _notifs.prepend(notification);
    }
        
    endInsertRows();

    _loading = false;
}
