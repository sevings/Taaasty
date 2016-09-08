#ifndef MESSAGEBASE_H
#define MESSAGEBASE_H

#include <QObject>
#include <QDateTime>

#include "TastyData.h"

class User;



class MessageBase : public TastyData
{
    Q_OBJECT

    Q_PROPERTY(QString  createdAt       MEMBER _createdAt      NOTIFY baseUpdated)
    Q_PROPERTY(QString  text            MEMBER _text           NOTIFY textUpdated)
    Q_PROPERTY(QString  truncatedText   MEMBER _truncatedText  NOTIFY textUpdated)
    Q_PROPERTY(User*    user            MEMBER _user           NOTIFY baseUpdated)
    Q_PROPERTY(bool     isRead          MEMBER _read           NOTIFY readChanged)
    Q_PROPERTY(bool     containsImage   MEMBER _containsImage  NOTIFY baseUpdated)

public:
    explicit MessageBase(QObject* parent = nullptr);

    bool        isRead() const;
    QDateTime   createdDate() const;

    QString text() const;

    User* user() const;

signals:
    void baseUpdated();
    void textUpdated();
    void readChanged();

protected:
    void _setTruncatedText();
    void _setDate(const QString d);

    QString     _createdAt;
    QDateTime   _date;
    QString     _text;
    QString     _truncatedText;
    User*       _user;
    bool        _read;
    bool        _containsImage;
};

#endif // MESSAGEBASE_H
