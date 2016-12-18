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

#ifndef NOTIFICATIONSMODEL_H
#define NOTIFICATIONSMODEL_H

#include <QJsonObject>
#include <QSet>

#include "tastylistmodel.h"

class Notification;
class Tasty;

#ifdef Q_OS_ANDROID
class AndroidNotifier;
#endif


class NotificationsModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(bool unread  READ unread  NOTIFY unreadChanged)

public:
    enum NotificationType
    {
        ActivityType        = 0,
        FriendActivityType  = 1
    };

    Q_ENUMS(NotificationType)

    NotificationsModel(NotificationType type = ActivityType, Tasty* tasty = nullptr);
    ~NotificationsModel();

    static NotificationsModel* instance(Tasty* tasty = nullptr);
    static NotificationsModel* friendActivity(Tasty* tasty = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    virtual bool hasMore() const override;

    bool unread() const;

signals:
    void unreadChanged();

public slots:
    void markAsRead();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _readSuccess();

    void _addItems(const QJsonObject& data);
    void _addPush(QJsonObject data);
    void _addNewest(const QJsonObject& data);

    void _reloadAll();
    void _check(int actual);

private:
    QList<Notification*> _notifs;
    QSet<int>            _ids;
    QString              _url;
    int                  _totalCount;

#ifdef Q_OS_ANDROID
    AndroidNotifier* _androidNotifier;
#endif
};

#endif // NOTIFICATIONSMODEL_H
