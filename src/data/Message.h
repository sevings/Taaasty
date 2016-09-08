#pragma once

#include <QObject>
#include <QJsonObject>

#include "MessageBase.h"

class Conversation;
class AttachedImagesModel;



class Message: public MessageBase
{
    Q_OBJECT

    friend class PusherClient;

    Q_PROPERTY(int      userId          MEMBER _userId         NOTIFY updated)
    Q_PROPERTY(int      recipientId     MEMBER _recipientId    NOTIFY updated)
    Q_PROPERTY(int      conversationId  MEMBER _conversationId NOTIFY updated)
    Q_PROPERTY(User*    replyTo         READ replyTo           NOTIFY updated)
    
    Q_PROPERTY(AttachedImagesModel* attachedImagesModel MEMBER _attachedImagesModel NOTIFY updated)
    
public:
    Message(QObject* parent = nullptr);
    Message(const QJsonObject data, Conversation* chat, QObject* parent = nullptr);
    ~Message();

    int   userId() const;
    User* replyTo();

signals:
    void updated();

public slots:
    void read();

private slots:
    void _init(const QJsonObject data);
    void _correctHtml();
    void _markRead(const QJsonObject data);

private:
    void _updateRead(const QJsonObject data);
    void _markRemoved(const QJsonObject data);

    int     _userId;
    int     _recipientId;
    int     _conversationId;
    User*   _replyTo;
    int     _replyUserId;

    Conversation* _chat;
    
    AttachedImagesModel* _attachedImagesModel;
};
