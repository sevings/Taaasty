#pragma once

#include <QObject>
#include <QJsonObject>

#include "../defines.h"

class User;



class Notification: public QObject
{
    Q_OBJECT

    friend class PusherClient;
    friend class NotificationsModel;
    
    Q_PROPERTY(int          id         MEMBER _id         CONSTANT)
    Q_PROPERTY(QString      createdAt  MEMBER _createdAt  CONSTANT)
    Q_PROPERTY(User*        sender     MEMBER _sender     CONSTANT)
    Q_PROPERTY(bool         read       MEMBER _read       NOTIFY read)
    Q_PROPERTY(QString      action     MEMBER _action     CONSTANT)
    Q_PROPERTY(QString      actionText MEMBER _actionText CONSTANT)
    Q_PROPERTY(QString      text       MEMBER _text       CONSTANT) //! \todo show updates
    Q_PROPERTY(int          entityId   MEMBER _entityId   CONSTANT)
    Q_PROPERTY(EntityType   entityType MEMBER _entityType CONSTANT)
    Q_PROPERTY(int          parentId   MEMBER _parentId   CONSTANT)
    Q_PROPERTY(QString      parentType MEMBER _parentType CONSTANT)
    
    Q_PROPERTY(Entry*       entry      READ entry         CONSTANT)

public:
    enum EntityType
    {
        UnknownType         = 0,
        EntryType           = 1,
        CommentType         = 2,
        RelationshipType    = 3
    };

    Q_ENUMS(EntityType)

    Notification(QObject* parent = nullptr);
    Notification(const QJsonObject data, QObject* parent = nullptr);
    ~Notification();
    
    int entityId() const;

    User* sender() const;

    QString actionText() const;

    QString text() const;

    int id() const;
    bool isRead() const;

    int parentId() const;

    Entry* entry();

signals:
    void read();
    
private:
    void _updateRead(const QJsonObject data);

    int         _id;
    QString     _createdAt;
    User*       _sender;
    bool        _read;
    QString     _action;
    QString     _actionText;
    QString     _text;
    int         _entityId;
    EntityType  _entityType;
    int         _parentId;
    QString     _parentType;
    //QString _image;

    EntryPtr    _entry;
};
