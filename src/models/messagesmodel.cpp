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

#include "messagesmodel.h"

// #include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../data/Message.h"
#include "../data/Notification.h"
#include "../data/Conversation.h"
#include "../apirequest.h"
#include "../tasty.h"

#include "notificationsmodel.h"

#ifdef Q_OS_ANDROID
#   include "../android/androidnotifier.h"
#   include "../data/User.h"
#endif



MessagesModel::MessagesModel(Conversation* chat)
    : TastyListModel(chat)
    , _chat(chat)
    , _chatId(0)
    , _url(QStringLiteral("v2/messenger/conversations/by_id/%1/messages.json?limit=%2&order=desc"))
#ifdef Q_OS_ANDROID
    , _androidNotifier(new AndroidNotifier(this))
#endif
{
    init(chat);
}



void MessagesModel::init(Conversation* chat)
{
    if (!chat || !chat->id() || _chatId)
        return;

    _chat = chat;
    _chatId = chat->id();
    _totalCount = chat->totalCount();

    emit hasMoreChanged();

    Q_TEST(connect(chat, SIGNAL(messageSent(QJsonObject)),      this, SLOT(_addMessage(QJsonObject))));
    Q_TEST(connect(chat, SIGNAL(messageReceived(QJsonObject)),  this, SLOT(_addMessage(QJsonObject))));
}



int MessagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _messages.size();
}



QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _messages.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Message*>(_messages.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool MessagesModel::hasMore() const
{
    return _messages.size() < _totalCount;
}



Message* MessagesModel::lastMessage() const
{
    if (_messages.isEmpty())
        return nullptr;

    return _messages.last();
}



void MessagesModel::reset()
{
    auto reset = !_messages.isEmpty();
    if (reset)
        beginResetModel();

    _chatId = _chat->id();

    qDeleteAll(_messages);
    _messages.clear();

    emit rowCountChanged();
    emit lastMessageChanged();

    delete _loadRequest;
    delete _checkRequest;

    _totalCount = _chat->totalCount();
    emit hasMoreChanged();

    if (reset)
        endResetModel();
}



void MessagesModel::check()
{
    if (isChecking() || !_chatId)
        return;

    QString url = _url.arg(_chatId).arg(20);
    if (!_messages.isEmpty())
        url += QStringLiteral("&from_message_id=%1").arg(_messages.last()->id());

    _checkRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    Q_TEST(connect(_checkRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addLastMessages(QJsonObject))));

    _initCheck();
}



void MessagesModel::loadMore()
{
    if (!hasMore())
    {
        emit hasMoreChanged();
        return;
    }

    if (isLoading() || !_chatId)
        return;

    int limit = qBound(20, _chat->unreadCount() - _messages.size(), 200);
    QString url = _url.arg(_chatId).arg(limit);
    if (!_messages.isEmpty())
        url += QStringLiteral("&to_message_id=%1").arg(_messages.first()->id());

    _loadRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError);
    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonObject)), this, SLOT(_addMessages(QJsonObject))));

    _initLoad();
}



QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "message";
    return roles;
}



void MessagesModel::_addMessages(const QJsonObject& data)
{
    auto feed = data.value(QStringLiteral("messages")).toArray();
    if (feed.isEmpty())
    {
        _totalCount = _messages.size();
        emit hasMoreChanged();
        return;
    }

    auto msgs = _messagesList(feed);
    if (msgs.isEmpty())
        return;

    emit itemsAboutToBePrepended();

    beginInsertRows(QModelIndex(), 0, msgs.size() - 1);

    _totalCount = data.value(QStringLiteral("total_count")).toInt();

    _messages = msgs + _messages;

    endInsertRows();

    emit rowCountChanged();

    if (_messages.size() <= msgs.size())
        emit lastMessageChanged();

    if (!hasMore())
        emit hasMoreChanged();

    emit itemsPrepended(msgs.size());
}



void MessagesModel::_addLastMessages(const QJsonObject& data)
{
    auto feed = data.value(QStringLiteral("messages")).toArray();
    if (feed.isEmpty())
        return;

    auto msgs = _messagesList(feed);
    if (msgs.isEmpty())
        return;

    beginInsertRows(QModelIndex(), _messages.size(), _messages.size() + msgs.size() - 1);

    _totalCount = data.value(QStringLiteral("total_count")).toInt();

    _messages << msgs;

    endInsertRows();

    emit rowCountChanged();
    emit lastMessageChanged();

    if (!hasMore())
        emit hasMoreChanged();
}



void MessagesModel::_addMessage(const QJsonObject& data)
{
    auto id = data.value(QStringLiteral("id")).toInt();
    if (_ids.contains(id))
        return;

    auto msg = new Message(data, _chat, this);

    _totalCount++;

    beginInsertRows(QModelIndex(), _messages.size(), _messages.size());

    _messages << msg;
    _ids << msg->id();

    Q_TEST(connect(msg, &QObject::destroyed, this, &MessagesModel::_removeMessage));
    Q_TEST(connect(msg, &MessageBase::readChanged, _chat, &Conversation::_decUnread));

    endInsertRows();

    emit rowCountChanged();
    emit lastMessageChanged();

    _chat->removeTyped(msg->userId());

#ifdef Q_OS_ANDROID
    if (!msg->isRead() && msg->userId() != _chat->userId() && pTasty->settings()->systemNotifications())
    {
        auto text = QStringLiteral("%1:\n%2").arg(msg->user()->name())
                .arg(msg->text());
        _androidNotifier->setNotification(text);
    }
#endif
}



void MessagesModel::_addMessage(const int chatId, const QJsonObject& data)
{
    if (chatId != _chatId)
        return;

    _addMessage(data);
}



void MessagesModel::_removeMessage(QObject* msg)
{
    auto message = static_cast<Message*>(msg);
    auto i = _messages.indexOf(message);
    if (i < 0)
        return;

    beginRemoveRows(QModelIndex(), i, i);

    _messages.removeAt(i);
    _ids.remove(message->id());


    endRemoveRows();

    emit rowCountChanged();

    _totalCount--;

    if (i == _messages.size())
        emit lastMessageChanged();
}



QList<Message*> MessagesModel::_messagesList(const QJsonArray& feed)
{
    QList<Message*> msgs;
    for (int i = 0; i < feed.size(); i++)
    {
        auto msg = new Message(feed.at(i).toObject(), _chat, this);
        if (_ids.contains(msg->id()))
            continue;

        _ids << msg->id();
        msgs.insert(i, msg);

        Q_TEST(connect(msg, &QObject::destroyed, this, &MessagesModel::_removeMessage));
        Q_TEST(connect(msg, &MessageBase::readChanged, _chat, &Conversation::_decUnread));
    }

    return msgs;
}
