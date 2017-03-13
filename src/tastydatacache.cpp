// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "tastydatacache.h"

#include "data/Message.h"
#include "data/Comment.h"
#include "data/Notification.h"



TastyDataCache::TastyDataCache()
{

}



void TastyDataCache::addChat(const ChatPtr& chat)
{
    if (!chat)
        return;

    Q_ASSERT(!chat->parent());

    _chats.insert(chat->id(), chat.data());

    chat->setCppOwnership();

    if (chat->entryId())
        _chatsByEntry.insert(chat->entryId(), chat.data());

    if (chat->type() == Conversation::PrivateConversation)
        _chatsByTlog.insert(chat->recipientId(), chat.data());
}



void TastyDataCache::removeChat(Conversation* chat)
{
    if (!chat)
        return;

    _chats.remove(chat->id());
    _chatsByEntry.remove(chat->entryId());
    _chatsByTlog.remove(chat->recipientId());
}



ChatPtr TastyDataCache::chat(int id) const
{
    auto chat = _chats.value(id);
    if (chat)
        return chat->sharedFromThis();

    return ChatPtr();
}



ChatPtr TastyDataCache::chatByEntry(int entryId) const
{
    auto chat = _chatsByEntry.value(entryId);
    if (chat)
        return chat->sharedFromThis();

    return ChatPtr();
}



ChatPtr TastyDataCache::chatByTlog(int tlogId) const
{
    auto chat = _chatsByTlog.value(tlogId);
    if (chat)
        return chat->sharedFromThis();

    return ChatPtr();
}



void TastyDataCache::addEntry(const EntryPtr& entry)
{
    Q_ASSERT(!entry || !entry->parent());

    entry->setCppOwnership();

    if (entry)
        _entries.insert(entry->entryId(), entry.data());
}



void TastyDataCache::removeEntry(int id)
{
    _entries.remove(id);
}



EntryPtr TastyDataCache::entry(int id) const
{
    auto entry = _entries.value(id);
    if (entry)
        return entry->sharedFromThis();

    return EntryPtr();
}



void TastyDataCache::addMessage(Message* msg)
{
    msg->setCppOwnership();

    _messages.insert(msg->id(), msg);
}



void TastyDataCache::removeMessage(int id)
{
    _messages.remove(id);
}



Message* TastyDataCache::message(int id) const
{
    return _messages.value(id);
}



void TastyDataCache::addComment(Comment* cmt)
{
    cmt->setCppOwnership();

    _comments.insert(cmt->id(), cmt);
}



void TastyDataCache::removeComment(int id)
{
    _comments.remove(id);
}



void TastyDataCache::addNotification(Notification* notif)
{
    notif->setCppOwnership();

    _notifications.insert(notif->id(), notif);
}



void TastyDataCache::removeNotification(int id)
{
    _notifications.remove(id);
}



Notification* TastyDataCache::notification(int id)
{
    return _notifications.value(id);
}
