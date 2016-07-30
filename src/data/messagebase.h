#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <QObject>

class User;



class MessageBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int      id              MEMBER _id             NOTIFY baseUpdated)
    Q_PROPERTY(QString  createdAt       MEMBER _createdAt      NOTIFY baseUpdated)
    Q_PROPERTY(QString  text            MEMBER _text           NOTIFY textUpdated)
    Q_PROPERTY(User*    user            MEMBER _user           NOTIFY baseUpdated)

public:
    explicit MessageBase(QObject* parent = nullptr);

    int id() const;

signals:
    void baseUpdated();
    void textUpdated();

protected:
    int     _id;
    QString _createdAt;
    QString _text;
    User*   _user;
};

#endif // MESSAGEBASE_H
