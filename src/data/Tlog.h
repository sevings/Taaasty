#pragma once

#include <QObject>
#include <QJsonObject>

#include "TastyData.h"

class Author;



class Tlog: public TastyData
{
    Q_OBJECT

    Q_PROPERTY(int          tlogId              READ tlogId   WRITE setId   NOTIFY updated) //! \todo remove
    Q_PROPERTY(QString      slug                READ slug   WRITE setSlug   NOTIFY updated)
    Q_PROPERTY(QString      title               MEMBER _title               NOTIFY updated)
    Q_PROPERTY(QString      entriesCount        MEMBER _entriesCount        NOTIFY updated)
    Q_PROPERTY(QString      publicEntriesCount  MEMBER _publicEntriesCount  NOTIFY updated)
    Q_PROPERTY(QString      privateEntriesCount MEMBER _privateEntriesCount NOTIFY updated)
    Q_PROPERTY(QString      daysCount           MEMBER _daysCount           NOTIFY updated)
    Q_PROPERTY(QString      followersCount      MEMBER _followersCount      NOTIFY updated)
    Q_PROPERTY(QString      followingsCount     MEMBER _followingsCount     NOTIFY updated)
    Q_PROPERTY(QString      ignoredCount        MEMBER _ignoredCount        NOTIFY updated)
    Q_PROPERTY(Relationship myRelationship      MEMBER _myRelation          NOTIFY updated)
    Q_PROPERTY(Relationship hisRelationship     MEMBER _hisRelation         NOTIFY updated)
    Q_PROPERTY(Author*      author              READ author                 NOTIFY updated)

public:
    enum Relationship {
        Undefined,
        Me,
        Friend,
        Ignored,
        None
    };

    Q_ENUMS(Relationship)

    Tlog(QObject* parent = nullptr);
    Tlog(const QJsonObject data, QObject* parent = nullptr);

    Author* author() const { return _author; }

public slots:
    int  tlogId() const                 { return _id; }
    void setId(const int id);

    QString slug() const                { return _slug; }
    void    setSlug(const QString slug);

    void init(const QJsonObject data);
    void reload();

signals:
    void updated();
    void loadingChanged();

private:
    Relationship _relationship(const QJsonObject& data, const QString field);

    QString         _slug;
    QString         _title;
    QString         _entriesCount;
    QString         _publicEntriesCount;
    QString         _privateEntriesCount;
    QString         _daysCount;
    QString         _followersCount;
    QString         _followingsCount;
    QString         _ignoredCount;
    Relationship    _myRelation;
    Relationship    _hisRelation;
    Author*         _author;
};
