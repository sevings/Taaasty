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
    Q_PROPERTY(QString  truncatedText   MEMBER _truncatedText  NOTIFY textUpdated)
    Q_PROPERTY(User*    user            MEMBER _user           NOTIFY baseUpdated)
    Q_PROPERTY(bool     isRead          MEMBER _read           NOTIFY readChanged)

public:
    explicit MessageBase(QObject* parent = nullptr);

    int id() const;

    bool isRead() const;

signals:
    void baseUpdated();
    void textUpdated();
    void readChanged();

protected:
    void _setTruncatedText();

    int     _id;
    QString _createdAt;
    QString _text;
    QString _truncatedText;
    User*   _user;
    bool    _read;
};

#endif // MESSAGEBASE_H
