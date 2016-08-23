#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QHash>
#include <QTimer>

class Pusher;
class Tasty;
class Conversation;
class Message;
class Comment;
class Notification;
class Entry;



class PusherClient : public QObject
{
    Q_OBJECT
public:
    explicit PusherClient(Tasty* tasty = nullptr);

    void addChat(Conversation* chat);
    void removeChat(Conversation* chat);

    Conversation* chat(int id) const;
    Conversation* chatByEntry(int entryId) const;

    void addEntry(Entry* entry);
    void removeEntry(int id);
    Entry* entry(int id) const;

    void addMessage(Message* msg);
    void removeMessage(int id);

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

    QHash<int, Conversation*>   _chats;
    QHash<int, Conversation*>   _chatsByEntry;
    QHash<int, Entry*>          _entries;
    QHash<int, Message*>        _messages;
    QHash<int, Comment*>        _comments;
    QHash<int, Notification*>   _notifications;
};

#endif // PUSHERCLIENT_H
