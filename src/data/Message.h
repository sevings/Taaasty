#pragma once

#include <QObject>
#include <QJsonObject>

#include "messagebase.h"

class Conversation;



class Message: public MessageBase
{
    Q_OBJECT

    friend class PusherClient;

    Q_PROPERTY(int      userId          MEMBER _userId         NOTIFY updated)
    Q_PROPERTY(int      recipientId     MEMBER _recipientId    NOTIFY updated)
    Q_PROPERTY(int      conversationId  MEMBER _conversationId NOTIFY updated)
    
public:
    Message(QObject* parent = nullptr);
    Message(const QJsonObject data, Conversation* chat, QObject* parent = nullptr);
    ~Message();

    int id() const;
    
    int userId() const;

public slots:
    void read();

signals:
    void updated();

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

    Conversation* _chat;
    // _attachments;
};
