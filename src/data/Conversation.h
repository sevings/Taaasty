#pragma once

#include <QObject>
#include <QJsonObject>
#include <QHash>

class MessageBase;
class Message;
class Entry;
class User;
class Author;
class MessagesModel;



class Conversation: public QObject
{
    Q_OBJECT

    friend class PusherClient;

    Q_PROPERTY(int              id              MEMBER _id              NOTIFY updated)
    Q_PROPERTY(ConversationType type            MEMBER _type            NOTIFY updated)
    Q_PROPERTY(int              unreadCount     MEMBER _unreadCount     NOTIFY unreadCountChanged)
    Q_PROPERTY(int              unreceivedCount MEMBER _unreceivedCount NOTIFY updated)
    Q_PROPERTY(int              totalCount      MEMBER _totalCount      NOTIFY updated)
    Q_PROPERTY(int              userId          MEMBER _userId          NOTIFY updated)
    Q_PROPERTY(int              recipientId     MEMBER _recipientId     NOTIFY updated)
    Q_PROPERTY(bool             isDisabled      MEMBER _isDisabled      NOTIFY updated)
    Q_PROPERTY(bool             notDisturb      MEMBER _notDisturb      NOTIFY updated)
    Q_PROPERTY(bool             isAnonymous     MEMBER _isAnonymous     NOTIFY updated)
    Q_PROPERTY(QString          topic           MEMBER _topic           NOTIFY updated)
    Q_PROPERTY(Entry*           entry           MEMBER _entry           NOTIFY updated)
//    Q_PROPERTY(QList<User*>     users           MEMBER _users           NOTIFY updated)
//    Q_PROPERTY(QList<User*>     deletedUsers    MEMBER _deletedUsers    NOTIFY updated)
    Q_PROPERTY(Author*          recipient       MEMBER _recipient       NOTIFY updated)
    Q_PROPERTY(MessagesModel*   messages        MEMBER _messages        NOTIFY updated)
    Q_PROPERTY(MessageBase*     lastMessage     READ lastMessage        NOTIFY lastMessageChanged)
    
public:
    enum ConversationType {
        UninitializedConversation,
        PublicConversation,
        GroupConversation,
        PrivateConversation
    };
    
    Q_ENUMS(ConversationType)

    Conversation(QObject* parent = nullptr);
    Conversation(Entry* entry);
    Conversation(const QJsonObject data, QObject* parent = nullptr);
    ~Conversation();

    int  id() const;
    void setId(int id);
    void setUserId(int id);
    void setSlug(const QString slug);
    void setEntryId(int entryId);

    int totalCount() const;

    bool isAnonymous() const;

    MessagesModel* messages() const;

    User* user(int id);

    MessageBase* lastMessage() const;

    ConversationType type() const;

public slots:
    void sendMessage(const QString text);
    void readAll();

signals:
    void updated();
    void unreadCountChanged();
    void loadingChanged();
    void messageSent(const QJsonObject);
    void messageReceived(const QJsonObject);
    void allMessagesRead(const QJsonObject data);
    void typed(int userId);
    void lastMessageChanged();

private slots:
    void _init(const QJsonObject data);
    void _setNotLoading();
    void _markRead(const QJsonObject data);

private:
    int                 _id;
    ConversationType    _type;
    int                 _unreadCount;
    int                 _unreceivedCount;
    int                 _totalCount;
    int                 _userId;
    int                 _recipientId;
    bool                _isDisabled;
    bool                _notDisturb;
    bool                _isAnonymous;
    QString             _topic;
    Entry*              _entry;
    QHash<int, User*>   _users;
    QHash<int, User*>   _deletedUsers;
    QHash<int, User*>   _leftUsers;
    Author*             _recipient;
    MessagesModel*      _messages;
    Message*            _lastMessage;

    bool _loading;
};
