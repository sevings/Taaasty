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
#include "../pusherclient.h"
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
    , _isAnonymous(false)
    , _entryId(0)
    , _recipient(nullptr)
    , _messages(nullptr)
    , _typedTimer(nullptr)
    , _hadTyped(false)
{
    
}



Conversation::~Conversation()
{
    Tasty::instance()->pusher()->removeChat(this);
}



void Conversation::setId(int id)
{
    if (isLoading() || id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    Tasty::instance()->pusher()->addChat(sharedFromThis());

    update();
}



void Conversation::setRecipientId(int id)
{
    if (isLoading() || id <= 0 || id == _recipientId)
        return;

    _recipientId = id;

    _request = new ApiRequest(QString("v2/messenger/conversations/by_user_id/%1.json").arg(_recipientId),
                              ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PostOperation);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::setSlug(const QString slug)
{
    if (isLoading() || slug.isEmpty())
        return;

    _request = new ApiRequest(QString("v2/messenger/conversations/by_slug/%1.json").arg(slug),
                              ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PostOperation);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::setEntryId(int entryId)
{
    if (isLoading() || entryId <= 0 || _entryId == entryId)
        return;

    _entryId = entryId;
    _entry.clear();

    Tasty::instance()->pusher()->addChat(sharedFromThis());

    auto data = QString("id=%1").arg(entryId);
    _request = new ApiRequest(QString("v2/messenger/conversations/by_entry_id.json"),
                              ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PostOperation, data);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::_markRead(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
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
    return  _lastMessage && _lastMessage->userId() == _userId
            && !_lastMessage->isRead();
}



MessagesModel* Conversation::messages()
{
    if (!_messages)
    {
        _messages = new MessagesModel(this);

        Q_TEST(connect(_messages, SIGNAL(lastMessageChanged()), this, SIGNAL(lastMessageChanged())));
    }

    return _messages;
}



User* Conversation::user(int id)
{
    if (_users.contains(id))
        return _users.value(id);

    if (id == _recipientId && _recipient)
        return _recipient;

    if (id == Tasty::instance()->settings()->userId())
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
        _users[id] = user;
        return user;
    }

    if (!_users.contains(0))
        _users[0] = new User(this);

    return _users[0];
}



MessageBase* Conversation::lastMessage()
{
    MessageBase* last = _lastMessage;

    if (_entryId)
    {
        auto lst = entry()->commentsModel()->lastComment();
        if (!last || (lst && lst->createdDate() > last->createdDate()))
            last = lst;
    }

    auto lst = messages()->lastMessage();
    if (!last || (lst && lst->createdDate() > last->createdDate()))
        last = lst;

    return last;
}



bool Conversation::isAnonymous() const
{
    return _isAnonymous;
}



int Conversation::totalCount() const
{
    return _totalCount;
}



void Conversation::init(const QJsonObject data)
{
     _id                = data.value("id").toInt();

     auto type = data.value("type").toString();
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

     _unreadCount       = data.value("unread_messages_count").toInt();
     _unreceivedCount   = data.value("unreceived_messages_count").toInt();
     _totalCount        = data.value("messages_count").toInt();
     _userId            = data.value("user_id").toInt();
     _recipientId       = data.value("recipient_id").toInt();
     _isDisabled        = data.value("is_disabled").toBool();
     _notDisturb        = data.value("not_disturb").toBool();
     _canTalk           = data.value("can_talk").toBool(true);
     _canDelete         = data.value("can_delete").toBool(true);
     _isAnonymous       = data.value("is_anonymous").toBool();

     Tasty::instance()->pusher()->addChat(sharedFromThis());

     messages()->init(this);

     if (!_entry && data.contains("entry"))
     {
         auto entryData = data.value("entry").toObject();
         _entryId = entryData.value("id").toInt();
         _entry = Tasty::instance()->pusher()->entry(_entryId);
         if (!_entry)
         {
             _entry = EntryPtr::create(this);
             _entry->init(entryData);
         }

         Tasty::instance()->pusher()->addChat(sharedFromThis());

         Q_TEST(connect(_entry->commentsModel(), SIGNAL(lastCommentChanged()), this, SIGNAL(lastMessageChanged())));
     }

     if (!_recipient && data.contains("recipient"))
         _recipient     = new Author(data.value("recipient").toObject(), this);

     if (data.contains("topic"))
         _topic         = data.value("topic").toString();
     else if (_recipient)
         _topic         = _recipient->name();
     else if (_entryId)
     {
         auto e = entry();
         _topic         = e->title().trimmed().isEmpty() ? e->text() : e->title();
         _topic = Tasty::truncateHtml(_topic);
         if (_topic.trimmed().isEmpty())
             _topic = QString("(запись %1)").arg(e->author()->name());
     }
     else
         _topic.clear();

     auto users = data.value("users").toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this); //! \todo isOnline
        _users.insert(user->id(), user);
     }

     users = data.value("users_deleted").toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this);
        _deletedUsers.insert(user->id(), user);
     }

     users = data.value("users_left").toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this);
        _leftUsers.insert(user->id(), user);
     }

     auto last = data.value("last_message").toObject();
     auto lastId = last.value("id").toInt();
     _lastMessage = Tasty::instance()->pusher()->message(lastId);
     if (!_lastMessage)
         _lastMessage   = new Message(last, this, this);

     emit idChanged();
     emit isInvolvedChanged();
     emit unreadCountChanged();
     emit lastMessageChanged();
     emit updated();
}



void Conversation::update()
{
    if (_id <= 0 || isLoading())
        return;

    _request = new ApiRequest(QString("/v2/messenger/conversations/by_id/%1.json").arg(_id),
                              ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject))));

    _initRequest();
}



void Conversation::sendMessage(const QString text)
{
    if (_sendRequest || _id <= 0)
        return;

    _hadTyped = false;
    if (_typedTimer)
        _typedTimer->stop();

    auto content = QUrl::toPercentEncoding(text.trimmed());
    auto uuid    = QUuid::createUuid().toString().remove('{').remove('}');
    auto data    = QString("uuid=%1&content=%2").arg(uuid).arg(QString::fromUtf8(content));
    auto url     = QString("v2/messenger/conversations/by_id/%1/messages.json").arg(_id);
    _sendRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(_sendRequest, SIGNAL(success(const QJsonObject)),           this, SIGNAL(messageSent(const QJsonObject))));
    Q_TEST(connect(_sendRequest, SIGNAL(error(QNetworkReply::NetworkError)),   this, SIGNAL(sendingMessageError())));
    Q_TEST(connect(_sendRequest, SIGNAL(error(const int, const QString)),      this, SIGNAL(sendingMessageError())));

    if (_unreadCount > 0)
        Q_TEST(connect(_sendRequest, SIGNAL(success(QJsonObject)),   this, SLOT(readAll())));

    ChatsModel::instance()->addChat(sharedFromThis());
}



void Conversation::readAll()
{
    if (_reading || _unreadCount <= 0 || _id <= 0)// || !isInvolved())
        return;

    auto url = QString("v2/messenger/conversations/by_id/%1/messages/read_all.json").arg(_id);
    _reading = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PutOperation);

    Q_TEST(connect(_reading, SIGNAL(success(const QJsonObject)), this, SIGNAL(allMessagesRead(const QJsonObject))));
    Q_TEST(connect(_reading, SIGNAL(success(QJsonObject)),       this, SLOT(_markRead(QJsonObject))));
}



void Conversation::leave()
{
    if (isLoading() || !isInvolved())
        return;
    
    auto url = QString("v2/messenger/conversations/by_id/%1/leave.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::PutOperation);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_emitLeft(QJsonObject))));
    
    _initRequest();
}



void Conversation::remove()
{
    if (isLoading() || !isInvolved())
        return;

    auto url = QString("v2/messenger/conversations/by_id/%1.json").arg(_id);
    _request = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                              QNetworkAccessManager::DeleteOperation);

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



void Conversation::_emitLeft(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
    {
        qDebug() << "error leave chat" << _id;
        return;
    }

    auto user = _users.value(_userId);
    if (!user)
    {
        user = new User(this);
        *user = *Tasty::instance()->me();
    }

    _leftUsers.insert(user->id(), user);
    emit isInvolvedChanged();

    emit Tasty::instance()->info("Беседа удалена");

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
    
    auto url = QString("v2/messenger/conversations/by_id/%1/typed.json").arg(_id);
    _typedRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError,
                                   QNetworkAccessManager::PostOperation);
    
#ifdef QT_DEBUG    
    Q_TEST(connect(_typedRequest, static_cast<void(ApiRequest::*)(QJsonObject)>(&ApiRequest::success),
                   [](const QJsonObject data)
    {
        auto status = data.value("status").toString();
        if (status != "success")
            qDebug() << "Error sending typed:" << data;
    }));
#endif
}



int Conversation::userId() const
{
    return _userId;
}



int Conversation::recipientId() const
{
    return _recipientId;
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
        auto user = this->user(userId);
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
