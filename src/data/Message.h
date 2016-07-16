#pragma once

#include <QObject>
#include <QJsonObject>

//class Author;



class Message: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int      id              MEMBER _id             NOTIFY updated)
    Q_PROPERTY(int      userId          MEMBER _userId         NOTIFY updated)
    Q_PROPERTY(int      recipientId     MEMBER _recipientId    NOTIFY updated)
    Q_PROPERTY(int      conversationId  MEMBER _conversationId NOTIFY updated)
    Q_PROPERTY(bool     isRead          MEMBER _read           NOTIFY readChanged)
    Q_PROPERTY(QString  createdAt       MEMBER _createdAt      NOTIFY updated)
    Q_PROPERTY(QString  text            MEMBER _text           NOTIFY textUpdated)
//    Q_PROPERTY(Author*  author          MEMBER _author         NOTIFY updated)
    
public:
    Message(QObject* parent = nullptr);
    Message(const QJsonObject data, QObject* parent = nullptr);

    int id() const;
    
public slots:
    void read();

signals:
    void updated();
    void textUpdated();
    void readChanged();

private slots:
    void _init(const QJsonObject data);
    void _correctHtml();
    void _markRead(const QJsonObject data);

private:
    int     _id;
    int     _userId;
    int     _recipientId;
    int     _conversationId;
    bool    _read;
    QString _createdAt;
    QString _text;
//    Author* _author;
    // _attachments;
};
