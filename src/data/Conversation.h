#pragma once

#include <QObject>
#include <QJsonObject>

class Message;
class Entry;
class User;
class Author;



class Conversation: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int              id              MEMBER _id              NOTIFY updated)
    Q_PROPERTY(ConversationType type            MEMBER _type            NOTIFY updated)
    Q_PROPERTY(int              unreadCount     MEMBER _unreadCount     NOTIFY updated)
    Q_PROPERTY(int              unreceivedCount MEMBER _unreceivedCount NOTIFY updated)
    Q_PROPERTY(int              totalCount      MEMBER _totalCount      NOTIFY updated)
    Q_PROPERTY(int              userId          MEMBER _userId          NOTIFY updated)
    Q_PROPERTY(int              recipientId     MEMBER _recipientId     NOTIFY updated)
    Q_PROPERTY(bool             isDisabled      MEMBER _isDisabled      NOTIFY updated)
    Q_PROPERTY(bool             notDisturb      MEMBER _notDisturb      NOTIFY updated)
    Q_PROPERTY(bool             isAnonymous     MEMBER _isAnonymous     NOTIFY updated)
    Q_PROPERTY(Message*         lastMessage     MEMBER _lastMessage     NOTIFY updated)
    Q_PROPERTY(Entry*           entry           MEMBER _entry           NOTIFY updated)
    Q_PROPERTY(QList<User*>     users           MEMBER _users           NOTIFY updated)
    Q_PROPERTY(QList<User*>     deletedUsers    MEMBER _deletedUsers    NOTIFY updated)
    Q_PROPERTY(Author*          recipient       MEMBER _recipient       NOTIFY updated)
    
public:
    enum ConversationType {
        UninitializedConversation,
        PublicConversation,
        PrivateConversation
    };
    
    Q_ENUMS(ConversationType)

    Conversation(QObject* parent = nullptr);
    Conversation(const QJsonObject data, QObject* parent = nullptr);

    int  id() const;
    void setId(int id);
    void setUserId(int id);
    void setSlug(const QString slug);
    void setEntryId(int entryId);

    int totalCount() const;

signals:
    void updated();
    void loadingChanged();

protected slots:
    void _init(const QJsonObject data);
    void _setNotLoading();

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
    Message*            _lastMessage;
    Entry*              _entry;
    QList<User*>        _users;
    QList<User*>        _deletedUsers;
    Author*             _recipient;
    
    bool _loading;
};
