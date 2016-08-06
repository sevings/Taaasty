#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QHash>

class Pusher;
class Tasty;
class Conversation;
class Message;
class Comment;
class Notification;



class PusherClient : public QObject
{
    Q_OBJECT
public:
    explicit PusherClient(Tasty* tasty = nullptr);

    void addChat(Conversation* chat);
    void removeChat(int id);

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

public slots:

private slots:
    void _getPusherAuth();
    void _resubscribeToPrivate();
    void _subscribeToPrivate(const QJsonObject data);
    void _handlePrivatePusherEvent(const QString event, const QString data);

private:
    void _addPrivateChannel();

    Pusher* _pusher;
    QString _privateChannel;

    Tasty* _tasty;

    QHash<int, Conversation*>   _chats;
    QHash<int, Message*>        _messages;
    QHash<int, Comment*>        _comments;
    QHash<int, Notification*>   _notifications;
};

#endif // PUSHERCLIENT_H