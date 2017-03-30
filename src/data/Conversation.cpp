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

#include "Conversation.h"

#include <QUuid>
#include <QDebug>

#include "../apirequest.h"
#include "../tastydatacache.h"
#include "../tasty.h"
#include "../settings.h"

#include "Entry.h"
#include "Message.h"
#include "User.h"
#include "Author.h"
#include "Tlog.h"
#include "Comment.h"

#include "../models/messagesmodel.h"
#include "../models/commentsmodel.h"
#include "../models/chatsmodel.h"



Conversation::Conversation(QObject* parent)
    : TastyData(parent)
    , _type(UninitializedConversation)
    , _unreadCount(0)
    , _unreceivedCount(0)
    , _totalCount(0)
    , _userId(0)
    , _recipientId(0)
    , _isDisabled(false)
    , _notDisturb(false)
    , _canTalk(false)
    , _canDelete(false)
    , _isAnonymous(false)
    , _entryId(0)
    , _recipient(nullptr)
    , _messages(new MessagesModel(this))
    , _typedTimer(nullptr)
    , _hadTyped(false)
{
    Q_TEST(connect(this, &Conversation::lastMessageChanged,
        this, &Conversation::isMyLastMessageUnreadChanged));

    Q_TEST(connect(_messages, &MessagesModel::lastMessageChanged,
                   this, &Conversation::_setLastMessage));
}



Conversation::~Conversation()
{
    pTasty->dataCache()->removeChat(this);
}



void Conversation::setId(int id)
{
    if (isLoading() || id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    pTasty->dataCache()->addChat(sharedFromThis());

    update();
}



void Conversation::setRecipientId(int id)
{
    if (isLoading() || id <= 0 || id == _recipientId)
        return;

    _recipientId = id;

    auto url = QStringLiteral("v2/messenger/conversations/by_user_id/%1.json").arg(_recipientId);
    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->post();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::setSlug(const QString& slug)
{
    if (isLoading() || slug.isEmpty())
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_slug/%1.json").arg(slug);
    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->post();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::setEntryId(int entryId)
{
    if (isLoading() || entryId <= 0 || _entryId == entryId)
        return;

    _entryId = entryId;
    _entry.clear();

    pTasty->dataCache()->addChat(sharedFromThis());

    auto url = QStringLiteral("v2/messenger/conversations/by_entry_id.json");
    _request = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _request->addFormData("id", entryId);
    _request->post();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::_markRead(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")).toString() != "success")
    {
        qDebug() << "error read chat" << _id;
        return;
    }

    _unreadCount = 0;
    emit unreadCountChanged();
}



Conversation::ConversationType Conversation::type() const
{
    return _type;
}



bool Conversation::isInvolved() const
{
    if (_type == PrivateConversation)
        return true;

    if (_userId <= 0)
        return false;

    return _users.contains(_userId)
            && !_leftUsers.contains(_userId)
            && !_deletedUsers.contains(_userId);
}



bool Conversation::isMyLastMessageUnread() const
{
    const auto msg = lastMessage();
    return  msg && msg->user()
            && msg->user()->id() == _userId
            && !msg->isRead();
}



MessagesModel* Conversation::messages() const
{
    return _messages;
}



User* Conversation::user(int id, bool reloadUsers)
{
    if (_users.contains(id))
        return _users.value(id);

    if (_leftUsers.contains(id))
        return _leftUsers.value(id);

    if (_deletedUsers.contains(id))
        return _deletedUsers.value(id);

    if (id == _recipientId && _recipient)
        return _recipient;

    if (id == pTasty->settings()->userId())
    {
        auto user = new User(this);
        *user = *Tasty::instance()->me();
        _users.insert(id, user);
        return user;
    }

    if (!_isAnonymous)
    {
        auto user = new User(this);
        user->setId(id);
        _users.insert(id, user);
        return user;
    }

    if (reloadUsers)
        updateUsers();

    if (!_users.contains(0))
        _users[0] = new User(this);

    return _users[0];
}



MessageBase* Conversation::lastMessage() const
{
    return _lastMessage;
}



bool Conversation::isAnonymous() const
{
    return _isAnonymous;
}



int Conversation::totalCount() const
{
    return _totalCount;
}



void Conversation::init(const QJsonObject& data)
{
     _id                = data.value(QLatin1String("id")).toInt();

     auto type = data.value(QLatin1String("type")).toString();
     if (type == "PublicConversation")
         _type = PublicConversation;
     else if (type == "PrivateConversation")
         _type = PrivateConversation;
     else if (type == "GroupConversation")
         _type = GroupConversation;
     else
     {
         qDebug() << "Unsupported conversation type:" << type;
         _type = UninitializedConversation;
     }

     _unreadCount       = data.value(QLatin1String("unread_messages_count")).toInt();
     _unreceivedCount   = data.value(QLatin1String("unreceived_messages_count")).toInt();
     _totalCount        = data.value(QLatin1String("messages_count")).toInt();
     _userId            = data.value(QLatin1String("user_id")).toInt();
     _recipientId       = data.value(QLatin1String("recipient_id")).toInt();
     _isDisabled        = data.value(QLatin1String("is_disabled")).toBool();
     _notDisturb        = data.value(QLatin1String("not_disturb")).toBool();
     _canTalk           = data.value(QLatin1String("can_talk")).toBool(true);
     _canDelete         = data.value(QLatin1String("can_delete")).toBool(true);
     _isAnonymous       = data.value(QLatin1String("is_anonymous")).toBool();

     auto dataCache = pTasty->dataCache();
     dataCache->addChat(sharedFromThis());

     if (_messages)
         _messages->init(this);
     else
     {
         _messages = new MessagesModel(this);

         Q_TEST(connect(_messages, &MessagesModel::lastMessageChanged,
                        this, &Conversation::_setLastMessage));
     }

     if (!_entry && data.contains(QStringLiteral("entry")))
     {
         auto entryData = data.value(QLatin1String("entry")).toObject();
         _entry = dataCache->initEntry(entryData, false);
         _entryId = _entry->id();

         dataCache->addChat(sharedFromThis());

//         Q_TEST(connect(_entry->commentsModel(), SIGNAL(lastCommentChanged()), this, SIGNAL(lastMessageChanged())));
     }

     if (!_recipient && data.contains(QStringLiteral("recipient")))
         _recipient     = new Author(data.value(QLatin1String("recipient")).toObject(), this);

     if (data.contains(QStringLiteral("topic")))
         _topic         = data.value(QLatin1String("topic")).toString();
     else if (_recipient)
         _topic         = _recipient->name();
     else if (_entryId)
     {
         auto e = entry();
         _topic         = e->title().trimmed().isEmpty() ? e->text() : e->title();
         _topic = Tasty::truncateHtml(_topic);
         if (_topic.trimmed().isEmpty())
             _topic = QStringLiteral("(запись %1)").arg(e->author()->name());
     }
     else
         _topic.clear();

     auto users = data.value(QLatin1String("users")).toArray();
     _initUsers(users);

     users = data.value(QLatin1String("users_deleted")).toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this);
        _deletedUsers.insert(user->id(), user);
     }

     users = data.value(QLatin1String("users_left")).toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this);
        _leftUsers.insert(user->id(), user);
     }

     auto last = data.value(QLatin1String("last_message")).toObject();
     auto lastId = last.value(QLatin1String("id")).toInt();
     bool lastMsgChanged = !_lastMessage || _lastMessage->id() != lastId;
     _lastMessage = dataCache->message(lastId);
     if (!_lastMessage)
     {
         _lastMessage   = new Message(last, this, this);

         Q_TEST(connect(_lastMessage, &MessageBase::readChanged, this, &Conversation::isMyLastMessageUnreadChanged));
     }

     if (lastMsgChanged)
         emit lastMessageChanged();

     emit idChanged();
     emit isInvolvedChanged();
     emit unreadCountChanged();
     emit updated();
}



void Conversation::update()
{
    if (_id <= 0 || isLoading())
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _request->get();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::sendMessage(const QString& text)
{
    if (_sendRequest || _id <= 0)
        return;

    _hadTyped = false;
    if (_typedTimer)
        _typedTimer->stop();

    auto uuid    = QUuid::createUuid().toString().remove('{').remove('}');
    auto url     = QStringLiteral("v2/messenger/conversations/by_id/%1/messages.json").arg(_id);

    _sendRequest = new ApiRequest(url, ApiRequest::AllOptions);
    _sendRequest->addFormData("uuid", uuid);
    _sendRequest->addFormData("content", text.trimmed());
    _sendRequest->post();

    Q_TEST(connect(_sendRequest, SIGNAL(success(const QJsonObject)),                this, SIGNAL(messageSent(const QJsonObject))));
    Q_TEST(connect(_sendRequest, SIGNAL(networkError(QNetworkReply::NetworkError)), this, SIGNAL(sendingMessageError())));
    Q_TEST(connect(_sendRequest, SIGNAL(error(const int, const QString)),           this, SIGNAL(sendingMessageError())));

    if (_unreadCount > 0)
        Q_TEST(connect(_sendRequest, SIGNAL(success(QJsonObject)),   this, SLOT(readAll())));

    ChatsModel::instance()->addChat(sharedFromThis());
}



void Conversation::readTo(int id)
{
    if (_readRequest || _unreadCount <= 0 || _id <= 0 || id <= 0)// || !isInvolved())
        return;

    const auto& messages = _messages->messages();
    auto it = messages.crbegin();
    for (; it != messages.crend(); it++)
        if ((*it)->id() == id)
            break;

    if (it == messages.crbegin())
    {
        readAll();
        return;
    }

    QStringList ids;
    QList<Message*> readMessages;
    for (; it != messages.crend() && !(*it)->isRead(); it++)
    {
        ids << QString::number((*it)->id());
        readMessages << *it;
    }

    if (ids.isEmpty())
        return;

    auto url  = QStringLiteral("v2/messenger/conversations/by_id/%1/messages/read.json").arg(_id);
    _readRequest  = new ApiRequest(url, ApiRequest::AllOptions);
    _readRequest->addFormData("ids", ids.join(','));
    _readRequest->put();

    foreach (auto msg, readMessages)
        Q_TEST(connect(_readRequest, SIGNAL(success(QJsonObject)), msg, SLOT(_markRead(QJsonObject))));
}



void Conversation::readAll()
{
    if (_readRequest || _unreadCount <= 0 || _id <= 0)// || !isInvolved())
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1/messages/read_all.json").arg(_id);
    _readRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _readRequest->put();

    Q_TEST(connect(_readRequest, SIGNAL(success(const QJsonObject)), this, SIGNAL(allMessagesRead(const QJsonObject))));
    Q_TEST(connect(_readRequest, SIGNAL(success(QJsonObject)),       this, SLOT(_markRead(QJsonObject))));
}



void Conversation::leave()
{
    if (isLoading() || !isInvolved())
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1/leave.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->put();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_emitLeft(QJsonObject))));

    _initRequest();
}



void Conversation::remove()
{
    if (isLoading() || !isInvolved())
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->deleteResource();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_emitLeft(QJsonObject))));

    _initRequest();
}



void Conversation::sendTyped()
{
    if (!_typedTimer)
    {
        _typedTimer = new QTimer(this);
        _typedTimer->setInterval(5000);
        _typedTimer->setSingleShot(true);
        Q_TEST(connect(_typedTimer, &QTimer::timeout, this, &Conversation::_sendTyped));
    }

    if (_typedTimer->isActive())
    {
        qDebug() << _typedTimer->remainingTime();
         if (_typedTimer->remainingTime() < 1000 && _typedTimer->remainingTime() > 0)
            _hadTyped = true;
    }
    else
    {
        _typedTimer->start();
        _hadTyped = true;
        _sendTyped();
    }
}



void Conversation::_emitLeft(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")).toString() != "success")
    {
        qDebug() << "error leave chat" << _id;
        return;
    }

    auto user = _users.value(_userId);
    if (!user)
    {
        user = new User(this);
        *user = *pTasty->me();
    }

    _leftUsers.insert(user->id(), user);
    emit isInvolvedChanged();

    emit Tasty::instance()->info(QStringLiteral("Беседа удалена"));

    emit left(_id);
}



void Conversation::_decUnread(bool read)
{
    if (!read || !_unreadCount)
        return;

    if (_unreadCount == 1)
        readAll();

    _unreadCount--;
    emit unreadCountChanged();
}



void Conversation::_removeTypedUser()
{
    auto timer = qobject_cast<QTimer*>(sender());
    if (!timer)
        return;

    auto userId = _typedUsers.key(timer);
    removeTyped(userId);
}



void Conversation::_sendTyped()
{
    if (!_hadTyped || _typedRequest)
        return;

    _hadTyped = false;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1/typed.json").arg(_id);
    _typedRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _typedRequest->post();

#ifdef QT_DEBUG
    Q_TEST(connect(_typedRequest, static_cast<void(ApiRequest::*)(const QJsonObject&)>(&ApiRequest::success),
                   [](const QJsonObject& data)
    {
        auto status = data.value(QLatin1String("status")).toString();
        if (status != "success")
            qDebug() << "Error sending typed:" << data;
    }));
#endif
}



void Conversation::_initUsers(const QJsonArray& data)
{
    foreach(auto userData, data)
    {
        auto id = userData.toObject().value(QLatin1String("id")).toInt();
        if (_users.contains(id))
            continue;

        auto user = new User(userData.toObject(), this); //! \todo isOnline
        _users.insert(user->id(), user);
    }

    emit usersUpdated();
}



void Conversation::_setLastMessage()
{
    auto last = _messages ? _messages->lastMessage() : nullptr;
    if (!last)
        return;

    if (_lastMessage && _lastMessage->id() == last->id())
        return;

    _lastMessage = last;
    emit lastMessageChanged();
}



int Conversation::userId() const
{
    return _userId;
}



int Conversation::recipientId() const
{
    return _recipientId;
}



Author* Conversation::recipient() const
{
    return _recipient;
}



QString Conversation::typedUsers()
{
    QString typed;

    if (_typedUsers.isEmpty())
        return typed;

    if (_type == PrivateConversation)
        return "Печатает...";

    QStringList userNames;
    foreach (auto userId, _typedUsers.keys())
    {
        auto user = this->user(userId, false);
        if (!user->name().isEmpty())
            userNames << user->name();
    }

    if (userNames.isEmpty())
        return "Печатает...";

    typed += userNames.first();

    if (userNames.size() == 1)
        return typed + " печатает...";

    for (int i = 1; i < userNames.size() - 1; i++)
        typed += ", " + userNames.at(i);

    typed += " и " + userNames.last() + " печатают...";
    return typed;
}



bool Conversation::isTyped() const
{
    return !_typedUsers.isEmpty();
}



void Conversation::addTyped(int userId)
{
    auto timer = _typedUsers.value(userId);
    if (timer)
    {
        timer->start();
        return;
    }

    timer = new QTimer(this);
    timer->setInterval(7000); //! \note pusher event is every 5 sec
    timer->start();

    Q_TEST(connect(timer, &QTimer::timeout, this, &Conversation::_removeTypedUser));

    _typedUsers.insert(userId, timer);

    emit typedUsersChanged();
}



void Conversation::removeTyped(int userId)
{
    _typedUsers.remove(userId);
    emit typedUsersChanged();
}



void Conversation::updateUsers()
{
    if (_usersRequest || _id <= 0
            || (_type != GroupConversation && _type != PublicConversation))
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1/users.json").arg(_id);
    _usersRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _usersRequest->get();

    Q_TEST(connect(_usersRequest, SIGNAL(success(QJsonArray)), this, SLOT(_initUsers(QJsonArray))));
}



Entry* Conversation::entry()
{
    return _entry.data();
}



int Conversation::entryId() const
{
    return _entryId;
}



int Conversation::unreadCount() const
{
    return _unreadCount;
}
