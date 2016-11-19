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

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../pusherclient.h"
#include "../data/Author.h"
#include "../data/Entry.h"



ChatsModel* ChatsModel::instance(Tasty* tasty)
{
    static auto model = new ChatsModel(tasty);
    return model;
}



ChatsModel::ChatsModel(Tasty* tasty)
    : TastyListModel(tasty)
    , _mode(AllChatsMode)
    , _url("v2/messenger/conversations.json?limit=10&page=%1")
    , _page(1)
{
    qDebug() << "ChatsModel";

    Q_TEST(connect(tasty,           SIGNAL(authorized()),       this, SLOT(reset())));
    Q_TEST(connect(tasty->pusher(), SIGNAL(unreadChat()),       this, SLOT(loadUnread())));
    Q_TEST(connect(tasty->pusher(), SIGNAL(unreadChats(int)),   this, SLOT(_checkUnread(int))));
}



int ChatsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _chats.size();
}



QVariant ChatsModel::data(const QModelIndex &index, int role) const
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
    if (parent.isValid() || !Tasty::instance()->isAuthorized())
        return false;

    return _hasMore;
}



void ChatsModel::fetchMore(const QModelIndex& parent)
{
    if (isLoading() || parent.isValid() || !Tasty::instance()->isAuthorized())
        return;

    qDebug() << "ChatsModel::fetchMore";

    QString url = _url.arg(_page);
    _loadRequest = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError);

    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonArray)), this, SLOT(_addChats(QJsonArray))));

    _initLoad();
}



void ChatsModel::setMode(ChatsModel::Mode mode)
{
    if (mode == _mode)
        return;
    
    beginResetModel();

    _mode = mode;
    emit modeChanged();
    
    if (_allChats.isEmpty())
        _allChats = _chats;

    _chats.clear();

    switch (_mode)
    {
    case AllChatsMode:
        _chats = _allChats;
        break;
    case PrivateChatsMode:
        foreach (auto chat, _allChats)
            if (chat->type() == Conversation::PrivateConversation
                    || chat->type() == Conversation::GroupConversation)
                _chats << chat;
        break;
    case EntryChatsMode:
        foreach (auto chat, _allChats)
            if (chat->type() == Conversation::PublicConversation)
                _chats << chat;
        break;
    default:
        qDebug() << "Error ChatsModel::Mode" << mode;
    }
    
    endResetModel();
}



void ChatsModel::addChat(EntryPtr entry)
{
    if (!entry)
        return;

    auto chat = entry->chat()->sharedFromThis();
    addChat(chat);
}



void ChatsModel::addChat(ChatPtr chat)
{
    if (!chat)
        return;

    if (chat->id() <= 0)
    {
        Q_ASSERT(chat->isLoading());

        Q_TEST(connect(chat.data(), &Conversation::updated, this, &ChatsModel::_addChat));
        return;
    }

    if (_ids.contains(chat->id()))
    {
        bubbleChat(chat->id());
        return;
    }

    beginInsertRows(QModelIndex(), 0, 0);

    _allChats.prepend(chat);
    _chats.prepend(chat);
    _ids << chat->id();

    Q_TEST(connect(chat.data(), SIGNAL(left(int)), this, SLOT(_removeChat(int))));

    endInsertRows();
}



void ChatsModel::bubbleChat(int id)
{
    if (!_ids.contains(id))
        return;

    int i = 0;
    for (; i < _chats.size(); i++)
        if (id == _chats.at(i)->id())
            break;

    if (i >= _chats.size())
        return;

    int unread = 0;
    for (; unread < _chats.size(); unread++)
        if (_chats.at(unread)->unreadCount() <= 0) //! \todo newly created
            break;

    if (i <= unread)
        return;

    if (!beginMoveRows(QModelIndex(), i, i, QModelIndex(), unread))
        return;

    auto chat = _chats.takeAt(i);
    _chats.insert(unread, chat);

    endMoveRows();
}



void ChatsModel::loadUnread()
{
    if (isChecking())
        return;

    qDebug() << "ChatsModel::loadUnread";

    QString url("v2/messenger/conversations.json?unread=true");
    _checkRequest = new ApiRequest(url, _optionsForFetchMore());

    Q_TEST(connect(_checkRequest, SIGNAL(success(QJsonArray)), this, SLOT(_addUnread(QJsonArray))));

    _initCheck();
}



void ChatsModel::reset()
{
    beginResetModel();

    _allChats.clear();
    _chats.clear();        
    _ids.clear();

    _hasMore = true;
    delete _checkRequest;
    delete _loadRequest;
    _page = 1;

    endResetModel();

    emit hasMoreChanged();
}



QHash<int, QByteArray> ChatsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "chat";
    return roles;
}



void ChatsModel::_addUnread(QJsonArray data)
{
    qDebug() << "ChatsModel::_addUnread";

    emit Tasty::instance()->pusher()->unreadChats(data.size());
    
    if (data.isEmpty())
        return;

    QList<int> bubbleIds;
    QList<ChatPtr> chats;
    foreach(auto item, data)
    {
        auto id = item.toObject().value("id").toInt();
        auto chat = Tasty::instance()->pusher()->chat(id);
        if (!chat)
        {
            chat = ChatPtr::create((QObject*)nullptr);
            chat->init(item.toObject());
        }

        if (_ids.contains(id))
        {
            if (_mode == AllChatsMode
                    || (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
                    || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation))
                bubbleIds << chat->id();

            continue;
        }

        _allChats << chat;
        _ids << chat->id();

        Q_TEST(connect(chat.data(), SIGNAL(left(int)), this, SLOT(_removeChat(int))));

        if (_mode == AllChatsMode
                || (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
                || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation))
            chats << chat;
    }

    foreach (auto id, bubbleIds)
        bubbleChat(id);

    if (chats.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, chats.size() - 1);

    for (int i = 0; i < chats.size(); i++)
        _chats.insert(i, chats.at(i));

    endInsertRows();
}



void ChatsModel::_addChats(QJsonArray data)
{
    qDebug() << "ChatsModel::_addChats";

    _page++;

    if (data.isEmpty())
    {
        _hasMore = false;
        emit hasMoreChanged();

        return;
    }

    QList<ChatPtr> chats;
    foreach(auto item, data)
    {
        auto id = item.toObject().value("id").toInt();
        if (_ids.contains(id))
            continue;

        auto chat = Tasty::instance()->pusher()->chat(id);
        if (!chat)
            chat = ChatPtr::create((QObject*)nullptr);

        chat->init(item.toObject());

        _ids << chat->id();
        _allChats << chat;

        Q_TEST(connect(chat.data(), SIGNAL(left(int)), this, SLOT(_removeChat(int))));

        if (_mode == AllChatsMode
                || (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
                || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation))
            chats << chat;
    }

    if (chats.isEmpty())
    {
        _loadRequest = nullptr;
        fetchMore(QModelIndex());
        return;
    }

    beginInsertRows(QModelIndex(), _chats.size(), _chats.size() + chats.size() - 1);
    _chats << chats;
    endInsertRows();
    
    if (_chats.size() < 10) //! \todo what is this?
        emit hasMoreChanged();
}



void ChatsModel::_removeChat(int id)
{
    if (!_ids.contains(id))
        return;

    _ids.remove(id);

    int i;
    for (i = 0; i < _allChats.size(); i++)
        if (_allChats.at(i)->id() == id)
        {
            _allChats.removeAt(i);
            break;
        }

    for (i = 0; i < _chats.size(); i++)
        if (_chats.at(i)->id() == id)
            break;
        
    if (i >= _chats.size())
        return;
    
    beginRemoveRows(QModelIndex(), i, i);
    
    _chats.removeAt(i);
    
    endRemoveRows();
}



void ChatsModel::_checkUnread(int actual)
{
    int found = 0;
    QList<int> bubbleIds;
    for (int i = 0; i < _chats.size(); i++)
    {
        if (_chats.at(i)->unreadCount() <= 0)
            continue;

        if (i > actual)
            bubbleIds << _chats.at(i)->id();

        found++;
        if (found >= actual)
            break;
    }

    foreach (auto id, bubbleIds)
        bubbleChat(id);

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
