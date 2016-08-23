#pragma once

#include <QObject>
#include <QJsonObject>

class Entry;
class EntryBase;



class CalendarEntry: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int     id             MEMBER _id             CONSTANT)
    Q_PROPERTY(QString createdAt      MEMBER _createdAt      CONSTANT)
    Q_PROPERTY(QString month          MEMBER _month          CONSTANT)
    Q_PROPERTY(QString date           MEMBER _date           CONSTANT)
    Q_PROPERTY(QString url            MEMBER _url            CONSTANT)
    Q_PROPERTY(bool    isPrivate      MEMBER _isPrivate      CONSTANT)
    Q_PROPERTY(QString type           MEMBER _type           CONSTANT)
    Q_PROPERTY(int     commentsCount  MEMBER _commentsCount  CONSTANT)
    Q_PROPERTY(QString truncatedTitle MEMBER _truncatedTitle CONSTANT)
    Q_PROPERTY(bool    isFlow         MEMBER _isFlow         CONSTANT)

public:
    CalendarEntry(QObject* parent = nullptr);
    CalendarEntry(const QJsonObject data, QObject* parent = nullptr);

    Entry* full();
    EntryBase* base();

    int     id() const;
    QString month() const;

private:
    int     _id;
    QString _createdAt;
    QString _month;
    QString _date;
    QString _url;
    bool    _isPrivate;
    QString _type;
    int     _commentsCount;
    QString _truncatedTitle;
    bool    _isFlow;

    Entry* _entry;
    EntryBase* _base;
};
