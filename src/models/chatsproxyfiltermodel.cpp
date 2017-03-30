// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "chatsproxyfiltermodel.h"

#include "../data/Conversation.h"
#include "../data/MessageBase.h"



ChatsProxyFilterModel::ChatsProxyFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , _mode(AllChatsMode)
{
    auto chats = pChats;
    chats->setFilter(this);

    Q_TEST(connect(chats, &ChatsModel::loadingChanged,      this, &ChatsProxyFilterModel::loadingChanged));
    Q_TEST(connect(chats, &ChatsModel::checkingChanged,     this, &ChatsProxyFilterModel::checkingChanged));
    Q_TEST(connect(chats, &ChatsModel::hasMoreChanged,      this, &ChatsProxyFilterModel::hasMoreChanged));
    Q_TEST(connect(chats, &ChatsModel::rowCountChanged,     this, &ChatsProxyFilterModel::rowCountChanged));
    Q_TEST(connect(chats, &ChatsModel::errorStringChanged,  this, &ChatsProxyFilterModel::errorStringChanged));
    Q_TEST(connect(chats, &ChatsModel::networkErrorChanged, this, &ChatsProxyFilterModel::networkErrorChanged));
}



void ChatsProxyFilterModel::setMode(const Mode mode)
{
    if (_mode == mode)
        return;

    _mode = mode;
    invalidateFilter();
    pChats->pendingSort();
    emit modeChanged();
}



bool ChatsProxyFilterModel::isLoading() const
{
    return pChats->isLoading();
}



bool ChatsProxyFilterModel::isChecking() const
{
    return pChats->isChecking();
}



bool ChatsProxyFilterModel::hasMore() const
{
    return pChats->hasMore();
}



bool ChatsProxyFilterModel::networkError() const
{
    return pChats->networkError();
}



QString ChatsProxyFilterModel::errorString() const
{
    return pChats->errorString();
}



void ChatsProxyFilterModel::loadLast()
{
    pChats->loadLast();
}



void ChatsProxyFilterModel::loadUnread()
{
    pChats->loadUnread();
}



void ChatsProxyFilterModel::reset()
{
    pChats->reset();
}



void ChatsProxyFilterModel::loadMore()
{
    pChats->loadMore();
}



bool ChatsProxyFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (!sourceModel())
        return false;

    if (_mode == AllChatsMode)
        return true;

    auto chat = sourceModel()->data(sourceModel()->index(sourceRow, 0, sourceParent), Qt::UserRole).value<Conversation*>();
    if (!chat)
        return false;

    return (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
            || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation);
}



bool ChatsProxyFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if (!sourceModel())
        return false;

    auto lchat = sourceModel()->data(left,  Qt::UserRole).value<Conversation*>();
    auto rchat = sourceModel()->data(right, Qt::UserRole).value<Conversation*>();

    if (!lchat)
        return rchat;

    if (!rchat)
        return false;

    if ((lchat->unreadCount() > 0) == (rchat->unreadCount() > 0))
        return lchat->lastMessage()->createdDate() < rchat->lastMessage()->createdDate();

    return lchat->unreadCount() < rchat->unreadCount();
}
