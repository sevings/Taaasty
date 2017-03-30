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

#include "chatsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>
#include <QQmlEngine>

#include "../tasty.h"
#include "../apirequest.h"
#include "../pusherclient.h"
#include "../tastydatacache.h"
#include "../statuschecker.h"
#include "../data/Author.h"
#include "../data/Entry.h"
#include "../data/Conversation.h"



ChatsModel* ChatsModel::instance(Tasty* tasty)
{
    static auto model = new ChatsModel(tasty);
    return model;
}



ChatsModel::ChatsModel(Tasty* tasty)
    : TastyListModel(tasty)
    , _statusChecker(new StatusChecker(this))
    , _url(QStringLiteral("v2/messenger/conversations.json?limit=10&page=%1"))
    , _page(1)
{
    qDebug() << "ChatsModel";

    Q_TEST(connect(tasty,           &Tasty::authorizedChanged,  this, &ChatsModel::reset));
    Q_TEST(connect(tasty->pusher(), &PusherClient::unreadChat,  this, &ChatsModel::loadUnread));
    Q_TEST(connect(tasty->pusher(), &PusherClient::unreadChats, this, &ChatsModel::_checkUnread));
}



int ChatsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _chats.size();
}



QVariant ChatsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _chats.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Conversation*>(_chats.at(index.row()).data());

    qDebug() << "role" << role;

    return QVariant();
}



bool ChatsModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid() || !pTasty->isAuthorized())
        return false;

    return _hasMore;
}



void ChatsModel::fetchMore(const QModelIndex& parent)
{
    if (isLoading() || parent.isValid() || !pTasty->isAuthorized())
        return;

    qDebug() << "ChatsModel::fetchMore";

    QString url = _url.arg(_page);
    _loadRequest = new ApiRequest(url, _optionsForFetchMore());

    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonArray)), this, SLOT(_addChats(QJsonArray))));

    _initLoad();
}



void ChatsModel::addChat(const EntryPtr& entry)
{
    if (!entry)
        return;

    auto chat = entry->chat()->sharedFromThis();
    addChat(chat);
}



void ChatsModel::addChat(const ChatPtr& chat)
{
    if (!chat)
        return;

    if (chat->id() <= 0)
    {
        Q_ASSERT(chat->isLoading());

        Q_TEST(connect(chat.data(), &Conversation::updated, this, &ChatsModel::_addChat));
        return;
    }

    if (!_insertChat(chat))
        return;

    beginInsertRows(QModelIndex(), 0, 0);
    _chats.prepend(chat);
    endInsertRows();

    emit rowCountChanged();
}



void ChatsModel::loadLast()
{
    if (isChecking() || !pTasty->isAuthorized())
        return;

    qDebug() << "ChatsModel::loadLast";

    _checkRequest = new ApiRequest(_url.arg(1), ApiRequest::AccessTokenRequired);

    Q_TEST(connect(_checkRequest, SIGNAL(success(QJsonArray)), this, SLOT(_addLast(QJsonArray))));
    Q_TEST(connect(_checkRequest, static_cast<void(ApiRequest::*)(const QJsonArray&)>(&ApiRequest::success),
                   this, [this]()
    {
        _checkUnread(pTasty->unreadChats());
    }, Qt::QueuedConnection));

    _initCheck();
}



void ChatsModel::loadUnread()
{
    if (isChecking())
        return;

    qDebug() << "ChatsModel::loadUnread";

    QString url(QStringLiteral("v2/messenger/conversations.json?unread=true"));
    _checkRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired);

    Q_TEST(connect(_checkRequest, SIGNAL(success(QJsonArray)), this, SLOT(_addUnread(QJsonArray))));

    _initCheck();
}



void ChatsModel::reset()
{
    beginResetModel();

    _chats.clear();
    _ids.clear();
    _entryChats.clear();
    _statusChecker->clear();

    _hasMore = true;
    delete _checkRequest;
    delete _loadRequest;
    _page = 1;

    endResetModel();

    emit rowCountChanged();
    emit hasMoreChanged();
}



QHash<int, QByteArray> ChatsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "chat";
    return roles;
}



void ChatsModel::_addLast(const QJsonArray& data)
{
    qDebug() << "ChatsModel::_addLast";

    auto chats = _chatList(data);
    if (chats.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, chats.size() - 1);

    for (int i = 0; i < chats.size(); i++)
        _chats.insert(i, chats.at(i));

    endInsertRows();
}



void ChatsModel::_addUnread(const QJsonArray& data)
{
    qDebug() << "ChatsModel::_addUnread";

    emit pTasty->pusher()->unreadChats(data.size());

    auto chats = _chatList(data);
    if (chats.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, chats.size() - 1);

    for (int i = 0; i < chats.size(); i++)
        _chats.insert(i, chats.at(i));

    endInsertRows();

    emit rowCountChanged();
}



void ChatsModel::_addChats(const QJsonArray& data)
{
    qDebug() << "ChatsModel::_addChats";

    _page++;

    if (data.isEmpty())
    {
        _hasMore = false;
        emit hasMoreChanged();

        return;
    }

    auto chats = _chatList(data);
    if (chats.isEmpty())
        return;

    beginInsertRows(QModelIndex(), _chats.size(), _chats.size() + chats.size() - 1);
    _chats << chats;
    endInsertRows();

    emit rowCountChanged();

    // requested 10 chats, but loaded less
    if (_chats.size() < 10) //! \todo set _hasMore to false?
        emit hasMoreChanged();
}



void ChatsModel::_removeChat(int id)
{
    if (!_ids.contains(id))
        return;

    _ids.remove(id);

    int i;
    for (i = 0; i < _chats.size(); i++)
        if (_chats.at(i)->id() == id)
            break;

    if (i >= _chats.size())
        return;

    beginRemoveRows(QModelIndex(), i, i);

    auto chat = _chats.takeAt(i);
    _entryChats.remove(chat->entryId());
    _statusChecker->remove(chat->recipient());

    endRemoveRows();

    emit rowCountChanged();
}



void ChatsModel::_checkUnread(int actual)
{
    int found = 0;
    for (int i = 0; i < _chats.size(); i++)
    {
        if (_chats.at(i)->unreadCount() <= 0)
            continue;

        found++;
        if (found >= actual)
            break;
    }

    if (found < actual)
        loadUnread();
}



void ChatsModel::_addChat()
{
    auto chat = qobject_cast<Conversation*>(sender());
    if (!chat || chat->id() <= 0)
        return;

    disconnect(chat, &Conversation::updated, this, &ChatsModel::_addChat);

    addChat(chat->sharedFromThis());
}



// if entry chat id has changed, remove old chat and insert new one
void ChatsModel::_insertEntryChat(const ChatPtr& chat)
{
    if (chat->type() != Conversation::PublicConversation)
        return;

    if (_entryChats.contains(chat->entryId()))
        _removeChat(_entryChats.value(chat->entryId()));

    if (chat->entry())
        chat->entry()->resetChat();

    _entryChats.insert(chat->entryId(), chat->id());
}



bool ChatsModel::_insertChat(const ChatPtr& chat)
{
    if (_ids.contains(chat->id()))
        return false;

    _insertEntryChat(chat);

    _ids << chat->id();
    _statusChecker->add(chat->recipient());

    Q_TEST(connect(chat.data(), &Conversation::left, this, &ChatsModel::_removeChat));
    Q_TEST(connect(chat.data(), &Conversation::lastMessageChanged, this, &TastyListModel::pendingSort));

    return true;
}



QList<ChatPtr> ChatsModel::_chatList(const QJsonArray& data)
{
    QList<ChatPtr> chats;
    foreach(auto item, data)
    {
        auto chat = pTasty->dataCache()->initChat(item.toObject());
        if (_insertChat(chat))
            chats << chat;
    }
    return chats;
}
