#pragma once

#include <QObject>
#include <QJsonObject>

class Author;



class Tlog: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int          tlogId              READ tlogId   WRITE setId   NOTIFY updated)
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

    Q_PROPERTY(bool loading                     MEMBER _loading             NOTIFY loadingChanged)

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

    void reload();

signals:
    void updated();
    void loadingChanged();

private slots:
    void _init(const QJsonObject data);

private:
    Relationship _relationship(const QJsonObject& data, const QString field);

    int             _id;
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

    bool            _loading;
};
