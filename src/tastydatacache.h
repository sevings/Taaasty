#ifndef TASTYDATACACHE_H
#define TASTYDATACACHE_H

#include <QHash>

#include "data/Entry.h"
#include "data/Conversation.h"

class Message;
class Comment;
class Notification;



class TastyDataCache
{
public:
    explicit TastyDataCache();

    void addChat(const ChatPtr& chat);
    void removeChat(Conversation* chat);

    ChatPtr chat(int id) const;
    ChatPtr chatByEntry(int entryId) const;
    ChatPtr chatByTlog(int tlogId) const;

    void addEntry(const EntryPtr& entry);
    void removeEntry(int id);

    EntryPtr entry(int id) const;

    void addMessage(Message* msg);
    void removeMessage(int id);

    Message* message(int id) const;

    void addComment(Comment* cmt);
    void removeComment(int id);

    void addNotification(Notification* notif);
    void removeNotification(int id);

    Notification* notification(int id);

private:
    QHash<int, QPointer<Conversation>>  _chats;
    QHash<int, QPointer<Conversation>>  _chatsByEntry;
    QHash<int, QPointer<Conversation>>  _chatsByTlog;
    QHash<int, QPointer<Entry>>         _entries;
    QHash<int, Message*>                _messages;
    QHash<int, Comment*>                _comments;
    QHash<int, Notification*>           _notifications;
};

#endif // TASTYDATACACHE_H
