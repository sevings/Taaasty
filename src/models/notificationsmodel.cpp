#include "notificationsmodel.h"

#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"
#include "../data/Notification.h"
#include "../tasty.h"

#ifdef Q_OS_ANDROID
#   include "../androidnotifier.h"
#   include "../data/User.h"
#endif


NotificationsModel::NotificationsModel(QObject* parent)
    : QAbstractListModel(parent)
    , _url("messenger/notifications.json?limit=20")
    , _loading(false)
    , _totalCount(1)
#ifdef Q_OS_ANDROID
    , _androidNotifier(new AndroidNotifier(this))
#endif
{
    qDebug() << "NotificationsModel";

    _timer.setInterval(60000);
    _timer.setSingleShot(false);
    _timer.start();

    Q_TEST(connect(&_timer, SIGNAL(timeout()),              this, SLOT(_check())));
    Q_TEST(connect(Tasty::instance(), SIGNAL(authorized()), this, SLOT(_reloadAll())));
}



NotificationsModel::~NotificationsModel()
{

}



NotificationsModel*NotificationsModel::instance(QObject* parent)
{
    static auto model = new NotificationsModel(parent);
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
//    qDebug() << "fetch more";

    if (_loading || parent.isValid() || _notifs.size() >= _totalCount
            || !Tasty::instance()->isAuthorized())
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



bool NotificationsModel::unread() const
{
    return !_notifs.isEmpty() && !_notifs.first()->_read;
}



void NotificationsModel::markAsRead()
{
    if (_notifs.isEmpty() || !unread())
        return;
    
    QString url = "messenger/notifications/read.json";
    QString data = QString("last_id=%1").arg(_notifs.first()->_id);
    
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);
    Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_readSuccess())));
//    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SIGNAL(unreadChanged())));
}



QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "notification";
    return roles;
}



void NotificationsModel::_check()
{
    if (_loading || !Tasty::instance()->isAuthorized())
        return;

    _loading = true;

    QString url = _url;
    if (!_notifs.isEmpty())
    {
        auto firstId = _notifs.first()->_id;
        url += QString("&from_notification_id=%1").arg(firstId);
    }
    
    auto request = new ApiRequest(url, true);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addNewest(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));
}



void NotificationsModel::_readSuccess()
{
    for (int i = 0; i < _notifs.size(); i++)
        if (!_notifs.at(i)->_read)
        {
            _notifs.at(i)->_read = true;
            emit _notifs.at(i)->read();
        }
        else
            break;

    emit unreadChanged();
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

//#ifdef Q_OS_ANDROID
//        auto text = QString("%1 %2\n%3").arg(notification->sender()->name())
//                .arg(notification->actionText()).arg(notification->text());
//        _androidNotifier->setNotification(text);
//#endif
    }

    endInsertRows();

    if (_notifs.size() >= _totalCount)
        emit hasMoreChanged();

    _loading = false;

    if (_notifs.size() <= list.size())
        emit unreadChanged();
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

#ifdef Q_OS_ANDROID
    if (!notification->_read)
    {
        auto text = QString("%1 %2\n%3").arg(notification->sender()->name())
                .arg(notification->actionText()).arg(notification->text());
        _androidNotifier->setNotification(text);
    }
#endif

        if (!notification->_read && notification->_action == "new_comment")
            emit commentAdded(notification->_parentId, notification);
    }
        
    endInsertRows();

    _loading = false;

    emit unreadChanged();
}



void NotificationsModel::_reloadAll()
{
    beginResetModel();

    _loading = false;
    _totalCount = 1;
    qDeleteAll(_notifs);

    endResetModel();

    fetchMore(QModelIndex());
}



void NotificationsModel::_setNotLoading()
{
    _loading = false;
}