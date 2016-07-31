#include "pusherclient.h"

#include "tasty.h"
#include "apirequest.h"

#include "qpusher/pusher.h"
#include "qpusher/channel.h"

#include "data/Conversation.h"
#include "data/Message.h"
#include "data/Comment.h"
#include "data/Notification.h"

#include "defines.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>



PusherClient::PusherClient(Tasty* tasty)
    : QObject(nullptr)
    , _pusher(new Pusher("40dbf1d864d4b366b5e6", this))
    , _tasty(tasty)
{
    if (!tasty)
        return;

    if (tasty->isAuthorized())
        _addPrivateChannel();

    Q_TEST(connect(tasty, SIGNAL(authorized()),         this,    SLOT(_resubscribeToPrivate())));
    Q_TEST(connect(tasty, SIGNAL(networkAccessible()),  _pusher, SLOT(connect())));
}



void PusherClient::addChat(Conversation* chat)
{
    _chats.insert(chat->id(), chat);
}



void PusherClient::removeChat(int id)
{
    _chats.remove(id);
}



void PusherClient::addMessage(Message* msg)
{
    _messages.insert(msg->id(), msg);
}



void PusherClient::removeMessage(int id)
{
    _messages.remove(id);
}



void PusherClient::addComment(Comment* cmt)
{
    _comments.insert(cmt->id(), cmt);
}



void PusherClient::removeComment(int id)
{
    _comments.remove(id);
}



void PusherClient::addNotification(Notification* notif)
{
    _notifications.insert(notif->id(), notif);
}



void PusherClient::removeNotification(int id)
{
    _notifications.remove(id);
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

    if (event.endsWith("status"))
    {
        auto count = json.value("unreadConversationsCount").toInt();

        qDebug() << "Unread chats:" << count;

        emit unreadChats(count);
        return;
    }

    if (event == "push_notification")
    {
        emit notification(json);
        return;
    }

    if (event == "typed")
    {
        auto chatId = json.value("conversation_id").toInt();
        auto userId = json.value("user_id").toInt();
        if (_chats.contains(chatId))
            emit _chats.value(chatId)->typed(userId);
        return;
    }

    if (event.endsWith("update_conversation"))
    {
        auto chatId = json.value("id").toInt();
        if (_chats.contains(chatId))
            _chats.value(chatId)->_init(json);
//        else
//            emit unreadChat();
        return;
    }

    if (event.endsWith("push_message"))
    {
        auto chatId = json.value("conversation_id").toInt();
        if (_chats.contains(chatId))
            emit _chats.value(chatId)->messageReceived(json);
        else
            emit unreadChat();
        return;
    }

    if (event.endsWith("update_messages"))
    {
        auto chatId = json.value("conversation_id").toInt();
        if (!_chats.contains(chatId))
            return;

        auto messages = json.value("messages").toArray();
        foreach (auto msgData, messages)
        {
            auto msgId = msgData.toObject().value("id").toInt();
            if (_messages.contains(msgId))
                _messages.value(msgId)->_updateRead(msgData.toObject());
        }

        return;
    }

    if (event == "update_notifications")
    {
// Data: "{\"notifications\":[{\"id\":9006398,\"read_at\":\"2016-07-19T21:25:21.000+03:00\"}]}"
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
