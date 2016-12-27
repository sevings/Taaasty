// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

#include "notificationsmodel.h"

#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"
#include "../data/Notification.h"
#include "../tasty.h"
#include "../pusherclient.h"

#ifdef Q_OS_ANDROID
#   include "../android/androidnotifier.h"
#   include "../data/User.h"
#   include "../settings.h"
#endif


NotificationsModel::NotificationsModel(NotificationType type, Tasty* tasty)
    : TastyListModel(tasty)
    , _totalCount(1)
#ifdef Q_OS_ANDROID
    , _androidNotifier(new AndroidNotifier(this))
#endif
{
    qDebug() << "NotificationsModel";

    switch (type)
    {
    case ActivityType:
        _url = "v2/messenger/notifications.json?limit=2";

        Q_TEST(connect(tasty,           &Tasty::unreadNotificationsChanged, this, &NotificationsModel::unreadChanged));
        Q_TEST(connect(tasty->pusher(), &PusherClient::notification,        this, &NotificationsModel::_addPush));
        Q_TEST(connect(tasty->pusher(), &PusherClient::unreadNotifications, this, &NotificationsModel::_check));
        break;
    case FriendActivityType:
        _url = "v2/messenger/notifications.json?type=FriendActivityNotification&include_entity=true&limit=2";
        break;
    default:
        qDebug() << "Unknown NotificationType: " << type;
    }

    Q_TEST(connect(tasty, &Tasty::authorizedChanged, this, &NotificationsModel::_reloadAll));
}



NotificationsModel::~NotificationsModel()
{

}



NotificationsModel* NotificationsModel::instance(Tasty* tasty)
{
    static auto model = new NotificationsModel(ActivityType, tasty);
    return model;
}



NotificationsModel* NotificationsModel::friendActivity(Tasty* tasty)
{
    static auto model = new NotificationsModel(FriendActivityType, tasty);
    return model;
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
    if (isLoading() || parent.isValid() || _notifs.size() >= _totalCount
            || !Tasty::instance()->isAuthorized())
        return;

    QString url = _url;
    if (!_notifs.isEmpty())
    {
        auto lastId = _notifs.last()->id();
        url += QString("0&to_notification_id=%1").arg(lastId); // load ten times more
    }

    _loadRequest = new ApiRequest(url, _optionsForFetchMore());

    connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));

    _initLoad();
}



bool NotificationsModel::hasMore() const
{
    return canFetchMore(QModelIndex());
}



bool NotificationsModel::unread() const
{
    return !_notifs.isEmpty() && Tasty::instance()->unreadNotifications() > 0;
}



void NotificationsModel::markAsRead()
{
    if (_notifs.isEmpty() || !unread())
        return;

    QString url("v2/messenger/notifications/read.json");
    auto request = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    request->addFormData("last_id", _notifs.first()->id());
    request->post();

    Q_TEST(connect(request, SIGNAL(success(QJsonArray)),  this, SLOT(_readSuccess())));
}



void NotificationsModel::check()
{
    if (isChecking() || _notifs.isEmpty() || !pTasty->isAuthorized())
        return;

    qDebug() << "NotificationsModel::check";

    QString url = _url + "00"; // limit 200
    if (!_notifs.isEmpty())
    {
        auto firstId = _notifs.first()->id();
        url += QString("&from_notification_id=%1").arg(firstId);
    }

    _checkRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired);

    Q_TEST(connect(_checkRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addNewest(QJsonObject))));

    _initCheck();
}



QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "notification";
    return roles;
}



 void NotificationsModel::_readSuccess()
 {
     for (auto it = _notifs.constBegin(); it != _notifs.constEnd(); ++it)
         if (!(*it)->isRead())
         {
             (*it)->_read = true;
             emit (*it)->readChanged();
         }
         else
             break;

     emit unreadChanged();
 }



void NotificationsModel::_addItems(const QJsonObject& data)
{
    int size = _notifs.size();
    auto list = data.value("notifications").toArray();
    if (list.isEmpty())
    {
        _totalCount = size;
        emit hasMoreChanged();
        return;
    }

    _totalCount = data.value("total_count").toInt();

    beginInsertRows(QModelIndex(), size, size + list.size() - 1);

    _notifs.reserve(size + list.size());
    foreach(auto notif, list)
    {
        auto notification = new Notification(notif.toObject(), this);
        _notifs.insert(size, notification);
        _ids << notification->id(); //! \todo check existing ids
    }

    endInsertRows();

    if (_notifs.size() >= _totalCount)
        emit hasMoreChanged();
}



void NotificationsModel::_addPush(QJsonObject data)
{
    auto id = data.value("id").toInt();
    if (_ids.contains(id))
        return;

    beginInsertRows(QModelIndex(), 0, 0);

    auto notification = new Notification(data, this);
    _notifs.prepend(notification);
    _ids << notification->id();

#ifdef Q_OS_ANDROID
    if (!notification->isRead() && Tasty::instance()->settings()->systemNotifications())
    {
        auto text = QString("%1 %2\n%3").arg(notification->sender()->name())
                .arg(notification->actionText()).arg(notification->text());
        _androidNotifier->setNotification(text);
    }
#endif

    endInsertRows();

    emit unreadChanged();
}



void NotificationsModel::_addNewest(const QJsonObject& data)
{
    auto list = data.value("notifications").toArray();
    if (list.isEmpty())
        return;

    _totalCount = data.value("total_count").toInt();

    QList<Notification*> notifs;
    foreach(auto notif, list)
    {
        auto obj = notif.toObject();
        auto id = obj.value("id").toInt();
        if (!_ids.contains(id))
        {
            notifs << new Notification(obj, this);
            _ids << id;
        }
    }

    beginInsertRows(QModelIndex(), 0, notifs.size() - 1);

    foreach(auto notification, notifs)
    {
        _notifs.prepend(notification);

#ifdef Q_OS_ANDROID
        if (!notification->isRead() && pTasty->settings()->systemNotifications())
        {
            auto text = QString("%1 %2\n%3").arg(notification->sender()->name())
                    .arg(notification->actionText()).arg(notification->text());
            _androidNotifier->setNotification(text);
        }
#endif
    }

    endInsertRows();

    emit unreadChanged();
}



void NotificationsModel::_reloadAll()
{
    beginResetModel();

    delete _loadRequest;
    delete _checkRequest;

    _totalCount = 1;
    qDeleteAll(_notifs);
    _notifs.clear();
    _ids.clear();

    endResetModel();

    fetchMore(QModelIndex());
}



void NotificationsModel::_check(int actual)
{
    if (actual <= 0)
        return;

    int unread = 0;
    foreach (auto notif, _notifs)
    {
        if (notif->isRead())
            break;
        else
            unread++;
    }

    if (unread < actual)
        check();
}
