#include "channel.h"

#include <QJsonObject>
#include <QDebug>

#include "pusher.h"



void Channel::subscribeToPrivate(const QString auth, const QString data)
{
    if (_subscribed)
        return;

    _sendSubscription(auth, data);
}



bool Channel::isSubscribed() const
{
    return _subscribed;
}



QString Channel::name() const
{
    return _name;
}



QSet<QString> Channel::presenceMemberIds() const
{
    return _presenceMemberIds;
}



void Channel::resubscribe()
{
    if (_subscribed)
        return;

    if (!_pusher->isConnected())
        return;

    qDebug() << "Subscribing to" << _name;

    if (_isPublic)
        _sendSubscription();
    else
        QMetaObject::invokeMethod(this, "authNeeded", Qt::QueuedConnection);
}



void Channel::unsubscribe()
{
    if (!_subscribed)
        return;

    if (!_sendUnsubscription())
        return;

    _clear();

    _pusher->unsubscribe(_name);
}



Channel::Channel(const QString name, bool isPublic, Pusher* pusher)
    : QObject(pusher)
    , _subscribed(false)
    , _isPublic(isPublic)
    , _name(name)
    , _pusher(pusher)
{
    Q_ASSERT(pusher);
}



bool Channel::_sendSubscription(const QString auth, const QString channelData)
{
    QJsonObject json;
    json["event"] = "pusher:subscribe";

    QJsonObject data;
    data["channel"] = _name;

    if (!auth.isEmpty())
        data["auth"] = auth;

    if (!channelData.isEmpty())
        data["channel_data"] = channelData;

    json["data"] = data;

    return _pusher->_send(json);
}



bool Channel::_sendUnsubscription()
{
    QJsonObject json;
    json["event"] = "pusher:unsubscribe";

    QJsonObject data;
    data["channel"] = _name;

    json["data"] = data;

    return _pusher->_send(json);
}



void Channel::_clear()
{
    _presenceMemberIds.clear();
    _subscribed = false;
    emit unsubscribed();
}
