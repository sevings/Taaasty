#pragma once

#include <QObject>
#include <QJsonObject>

class Author;



class Message: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int      id              MEMBER _id             NOTIFY updated)
    Q_PROPERTY(int      userId          MEMBER _userId         NOTIFY updated)
    Q_PROPERTY(int      recipientId     MEMBER _recipientId    NOTIFY updated)
    Q_PROPERTY(int      conversationId  MEMBER _conversationId NOTIFY updated)
    Q_PROPERTY(QString  createdAt       MEMBER _createdAt      NOTIFY updated)
    Q_PROPERTY(QString  content         MEMBER _content        NOTIFY updated)
    Q_PROPERTY(Author*  author          MEMBER _author         NOTIFY updated)
    
public:
    Message(QObject* parent = nullptr);
    Message(const QJsonObject data, QObject* parent = nullptr);

    int id() const;
    
signals:
    void updated();

private slots:
    void _init(const QJsonObject data);

private:
    int     _id;
    int     _userId;
    int     _recipientId;
    int     _conversationId;
    QString _createdAt;
    QString _content;
    Author* _author;
    // _attachments;
};
