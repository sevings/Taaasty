#include "notificationsmodel.h"

#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"
#include "../data/Notification.h"
#include "../tasty.h"
#include "../pusherclient.h"

#ifdef Q_OS_ANDROID
#   include "../androidnotifier.h"
#   include "../data/User.h"
#   include "../settings.h"
#endif


NotificationsModel::NotificationsModel(QObject* parent)
    : QAbstractListModel(parent)
    , _url("v2/messenger/notifications.json?limit=2")
    , _loading(false)
    , _checking(false)
    , _totalCount(1)
#ifdef Q_OS_ANDROID
    , _androidNotifier(new AndroidNotifier(this))
#endif
{
    qDebug() << "NotificationsModel";

    Q_TEST(connect(Tasty::instance(),           SIGNAL(unreadNotificationsChanged()), this, SIGNAL(unreadChanged())));
    Q_TEST(connect(Tasty::instance(),           SIGNAL(authorized()),                 this, SLOT(_reloadAll())));
    Q_TEST(connect(Tasty::instance()->pusher(), SIGNAL(notification(QJsonObject)),    this, SLOT(_addPush(QJsonObject))));
    Q_TEST(connect(Tasty::instance()->pusher(), SIGNAL(unreadNotifications(int)),     this, SLOT(_check(int))));
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
    if (_loading || parent.isValid() || _notifs.size() >= _totalCount
            || !Tasty::instance()->isAuthorized())
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url;
    if (!_notifs.isEmpty())
    {
        auto lastId = _notifs.last()->id();
        url += QString("0&to_notification_id=%1").arg(lastId); // load ten times more
    }
    
    auto request = new ApiRequest(url, true);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));
}



bool NotificationsModel::unread() const
{
    return !_notifs.isEmpty() && Tasty::instance()->unreadNotifications() > 0;
}



void NotificationsModel::markAsRead()
{
    if (_notifs.isEmpty() || !unread())
        return;
    
    QString url = "v2/messenger/notifications/read.json";
    QString data = QString("last_id=%1").arg(_notifs.first()->id());
    
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);
    Q_TEST(connect(request, SIGNAL(success(QJsonArray)),  this, SLOT(_readSuccess())));
}



QHash<int, QByteArray> NotificationsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "notification";
    return roles;
}



 void NotificationsModel::_readSuccess()
 {
     for (int i = 0; i < _notifs.size(); i++)
         if (!_notifs.at(i)->isRead())
         {
             _notifs.at(i)->_read = true;
             emit _notifs.at(i)->readChanged();
         }
         else
             break;

     emit unreadChanged();
 }



void NotificationsModel::_addItems(QJsonObject data)
{
    int size = _notifs.size();
    auto list = data.value("notifications").toArray();
    if (list.isEmpty())
    {
        _loading = false;
        emit loadingChanged();
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

    _loading = false;
    emit loadingChanged();
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



void NotificationsModel::_addNewest(const QJsonObject data)
{
    auto list = data.value("notifications").toArray();
    if (list.isEmpty())
    {
        _checking = false;
        return;
    }

    _totalCount = data.value("total_count").toInt();

    QList<Notification*> notifs;
    foreach(auto notif, list)
    {
        auto id = data.value("id").toInt();
        if (!_ids.contains(id))
        {
            notifs << new Notification(notif.toObject(), this);
            _ids << id;
        }
    }

    beginInsertRows(QModelIndex(), 0, notifs.size() - 1);

    foreach(auto notification, notifs)
    {
        _notifs.prepend(notification);

#ifdef Q_OS_ANDROID
        if (!notification->isRead() && Tasty::instance()->settings()->systemNotifications())
        {
            auto text = QString("%1 %2\n%3").arg(notification->sender()->name())
                    .arg(notification->actionText()).arg(notification->text());
            _androidNotifier->setNotification(text);
        }
#endif
    }

    endInsertRows();

    _loading = false;

    emit unreadChanged();
}



void NotificationsModel::_reloadAll()
{
    beginResetModel();

    _loading = false;
    emit loadingChanged();

    _totalCount = 1;
    qDeleteAll(_notifs);
    _notifs.clear();
    _ids.clear();

    endResetModel();

    fetchMore(QModelIndex());
}



void NotificationsModel::_check(int actual) //! \todo test me
{
    if (_checking || actual <= 0 || !Tasty::instance()->isAuthorized())
        return;

    int unread = 0;
    foreach (auto notif, _notifs)
    {
        if (notif->isRead())
            break;
        else
            unread++;
    }

    if (unread >= actual)
        return;

    qDebug() << "NotificationsModel::_check";

    _checking = true;

    QString url = _url + "00"; // limit 200
    if (!_notifs.isEmpty())
    {
        auto firstId = _notifs.first()->id();
        url += QString("&from_notification_id=%1").arg(firstId);
    }

    auto request = new ApiRequest(url, true);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addNewest(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotChecking())));
}



void NotificationsModel::_setNotLoading()
{
    _loading = false;
    emit loadingChanged();
}



void NotificationsModel::_setNotChecking()
{
    _checking = false;
}



bool NotificationsModel::loading() const
{
    return _loading;
}
