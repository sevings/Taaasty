#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QHash>
#include <QTimer>
#include <QWeakPointer>

#include "data/Entry.h"
#include "data/Conversation.h"

class Pusher;
class Tasty;
class Message;
class Comment;
class Notification;



class PusherClient : public QObject
{
    Q_OBJECT
public:
    explicit PusherClient(Tasty* tasty = nullptr);

    void addChat(ChatPtr chat);
    void removeChat(Conversation* chat);

    ChatPtr chat(int id) const;
    ChatPtr chatByEntry(int entryId) const;

    void addEntry(EntryPtr entry);
    void removeEntry(int id);
    EntryPtr entry(int id) const;

    void addMessage(Message* msg);
    void removeMessage(int id);
    Message* message(int id) const;

    void addComment(Comment* cmt);
    void removeComment(int id);

    void addNotification(Notification* notif);
    void removeNotification(int id);

signals:
    void notification(const QJsonObject data);
    void unreadChat();
    void unreadChats(int count);
    void unreadNotifications(int count);

public slots:
    void connect();
    void reconnect();

private slots:
    void _getPusherAuth();
    void _resubscribeToPrivate();
    void _subscribeToPrivate(const QJsonObject data);
    void _handlePrivatePusherEvent(const QString event, const QString data);
    void _sendReady();

private:
    void _addPrivateChannel();

    Pusher* _pusher;
    QString _privateChannel;

    Tasty* _tasty;

    QTimer _readyTimer;

    QHash<int, QWeakPointer<Conversation>>  _chats;
    QHash<int, QWeakPointer<Conversation>>  _chatsByEntry;
    QHash<int, QWeakPointer<Entry>>         _entries;
    QHash<int, Message*>                    _messages;
    QHash<int, Comment*>                    _comments;
    QHash<int, Notification*>               _notifications;
};

#endif // PUSHERCLIENT_H
