#include "pusherclient.h"

#include "tasty.h"
#include "apirequest.h"

#include "qpusher/pusher.h"
#include "qpusher/channel.h"

#include "data/Conversation.h"
#include "data/Message.h"
#include "data/Comment.h"
#include "data/Notification.h"
#include "data/Entry.h"

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

    _readyTimer.setInterval(300000);
    _readyTimer.setSingleShot(false);

    Q_TEST(QObject::connect(&_readyTimer, SIGNAL(timeout()), this, SLOT(_sendReady())));

    if (tasty->isAuthorized())
        _addPrivateChannel();

    Q_TEST(QObject::connect(tasty, SIGNAL(authorized()),         this, SLOT(_resubscribeToPrivate())));
    Q_TEST(QObject::connect(tasty, SIGNAL(networkAccessible()),  this, SLOT(connect())));
}



void PusherClient::addChat(Conversation* chat)
{
    _chats.insert(chat->id(), chat);
    _chatsByEntry.insert(chat->entryId(), chat);
}



void PusherClient::removeChat(Conversation* chat)
{
    _chats.remove(chat->id());
    _chatsByEntry.remove(chat->entryId());
}



Conversation*PusherClient::chat(int id) const
{
    return _chats.value(id);
}



Conversation* PusherClient::chatByEntry(int entryId) const
{
    return _chatsByEntry.value(entryId);
}



void PusherClient::addEntry(Entry* entry)
{
    _entries.insert(entry->entryId(), entry);
}



void PusherClient::removeEntry(int id)
{
    _entries.remove(id);
}



Entry*PusherClient::entry(int id) const
{
    return _entries.value(id);
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



void PusherClient::connect()
{
    if (!_pusher->isConnected())
        _pusher->connect();
}



void PusherClient::reconnect()
{
    _pusher->reconnect();
}



void PusherClient::_getPusherAuth()
{
    auto socket = _pusher->socketId();
    auto data = QString("socket_id=%1&channel_name=%2").arg(socket).arg(_privateChannel);

    auto request = new ApiRequest("v2/messenger/auth.json", true, QNetworkAccessManager::PostOperation, data);
    Q_TEST(QObject::connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_subscribeToPrivate(QJsonObject))));
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
}



void PusherClient::_handlePrivatePusherEvent(const QString event, const QString data)
{
    qDebug() << "Pusher event:" << event;

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
        foreach (auto chat, _chats.values(chatId))
            emit chat->typed(userId);
        return;
    }

    if (event.endsWith("update_conversation"))
    {
        auto chatId = json.value("id").toInt();
        foreach (auto chat, _chats.values(chatId))
            chat->_init(json);
        return;
    }

    if (event.endsWith("push_message"))
    {
        auto chatId = json.value("conversation_id").toInt();
        if (_chats.contains(chatId))
            foreach (auto chat, _chats.values(chatId))
                emit chat->messageReceived(json);
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

    if (event.endsWith("delete_user_messages"))
    {
        auto chatId = json.value("conversation_id").toInt();
        if (!_chats.contains(chatId))
            return;

        auto messages = json.value("messages").toArray();
        foreach (auto msgData, messages)
        {
            auto msgId = msgData.toObject().value("id").toInt();
            if (_messages.contains(msgId))
                _messages.value(msgId)->_markRemoved(msgData.toObject());
        }

        return;
    }

    if (event == "update_notifications")
    {
        auto notifs = json.value("notifications").toArray();
        foreach (auto notifData, notifs)
        {
            auto notifId = notifData.toObject().value("id").toInt();
            if (_notifications.contains(notifId))
                _notifications.value(notifId)->_updateRead(notifData.toObject());
        }

        return;
    }

    qDebug() << "Data:" << data;
}



void PusherClient::_sendReady()
{
    if (_pusher->isConnected())
    {
        auto data = QString("socket_id=%1").arg(_pusher->socketId());
        new ApiRequest("v2/messenger/only_ready.json", true,
                       QNetworkAccessManager::PostOperation, data);
    }
    else
        _pusher->connect();
}



void PusherClient::_addPrivateChannel()
{
    _privateChannel = QString("private-%1-messaging").arg(_tasty->settings()->userId());
    auto ch = _pusher->subscribe(_privateChannel, false);

    Q_TEST(QObject::connect(ch, SIGNAL(authNeeded()),           this, SLOT(_getPusherAuth())));
    Q_TEST(QObject::connect(ch, SIGNAL(subscribed()),           this, SLOT(_sendReady())));
    Q_TEST(QObject::connect(ch, SIGNAL(subscribed()),   &_readyTimer, SLOT(start())));
    Q_TEST(QObject::connect(ch, SIGNAL(event(QString,QString)), this, SLOT(_handlePrivatePusherEvent(QString,QString))));
}
