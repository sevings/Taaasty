#include "pusherclient.h"

#include "tasty.h"
#include "apirequest.h"

#include "qpusher/pusher.h"
#include "qpusher/channel.h"

#include "defines.h"

#include <QJsonDocument>
#include <QDebug>



PusherClient::PusherClient(Tasty* tasty)
    : QObject(tasty)
    , _pusher(new Pusher("40dbf1d864d4b366b5e6", this))
    , _tasty(tasty)
{
    if (!tasty)
        return;

    if (tasty->isAuthorized())
        _addPrivateChannel();

    Q_TEST(connect(tasty, SIGNAL(authorized()),         this, SLOT(_resubscribeToPrivate())));
    Q_TEST(connect(tasty, SIGNAL(networkAccessible()),  this, SLOT(_resubscribeToPrivate())));
}



void PusherClient::_getPusherAuth()
{
    auto socket = _pusher->socketId();
    auto data = QString("socket_id=%1&channel_name=%2").arg(socket).arg(_privateChannel);

    auto request = new ApiRequest("v1/messenger/auth.json", true, QNetworkAccessManager::PostOperation, data);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_subscribeToPrivate(QJsonObject)));
}



void PusherClient::_resubscribeToPrivate()
{
    _pusher->unsubscribe(_privateChannel);
    _addPrivateChannel();
}



void PusherClient::_subscribeToPrivate(const QJsonObject data)
{
    auto auth = data.value("auth").toString();
    _pusher->channel(_privateChannel)->subscribeToPrivate(auth);

    new ApiRequest("v1/messenger/only_ready.json", true, QNetworkAccessManager::PostOperation);
}



void PusherClient::_handlePrivatePusherEvent(const QString event, const QString data)
{
    QJsonParseError jpe;
    auto json = QJsonDocument::fromJson(data.toUtf8(), &jpe).object();
    if (jpe.error != QJsonParseError::NoError)
    {
        qDebug() << "parse error: " << jpe.errorString();
        qDebug() << "json:" << data;
        return;
    }


    if (event == "push_notification")
    {
        emit notification(json);
        return;
    }

    qDebug() << "Pusher event:" << event;
    qDebug() << "Data:" << data;
}



void PusherClient::_addPrivateChannel()
{
    _privateChannel = QString("private-%1-messaging").arg(_tasty->settings()->userId());
    auto ch = _pusher->subscribe(_privateChannel, false);

    Q_TEST(connect(ch, SIGNAL(authNeeded()),           this, SLOT(_getPusherAuth())));
    Q_TEST(connect(ch, SIGNAL(event(QString,QString)), this, SLOT(_handlePrivatePusherEvent(QString,QString))));
}
