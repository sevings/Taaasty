#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

#include "User.h"



class Author: public User
{
    Q_OBJECT

    Q_PROPERTY(bool    isFemale            MEMBER _isFemale            NOTIFY authorUpdated)
    Q_PROPERTY(bool    isPrivacy           MEMBER _isPrivacy           NOTIFY authorUpdated)
    Q_PROPERTY(bool    isOnline            MEMBER _isOnline            NOTIFY statusUpdated)
    Q_PROPERTY(bool    isFlow              MEMBER _isFlow              NOTIFY authorUpdated)
    Q_PROPERTY(bool    isPremium           MEMBER _isPremium           NOTIFY authorUpdated)
    Q_PROPERTY(bool    isDaylog            MEMBER _isDaylog            NOTIFY authorUpdated)
    Q_PROPERTY(QString title               MEMBER _title               NOTIFY authorUpdated)
    Q_PROPERTY(QString entriesCount        MEMBER _entriesCount        NOTIFY authorUpdated)
    Q_PROPERTY(QString publicEntriesCount  MEMBER _publicEntriesCount  NOTIFY authorUpdated)
    Q_PROPERTY(QString privateEntriesCount MEMBER _privateEntriesCount NOTIFY authorUpdated)
    Q_PROPERTY(QString daysCount           MEMBER _daysCount           NOTIFY authorUpdated)
    Q_PROPERTY(QString followingsCount     MEMBER _followingsCount     NOTIFY authorUpdated)
    Q_PROPERTY(QString lastSeenAt          MEMBER _lastSeenAt          NOTIFY statusUpdated)

public:
    Author(QObject* parent = nullptr);
    Author(const QJsonObject data, QObject* parent = nullptr);

    bool isFemale() const;
    bool isFlow() const;
    bool isPremium() const;
    bool isDaylog() const;

public slots:
    void init(const QJsonObject data);
    void checkStatus();
    void reload();

signals:
    void authorUpdated();
    void statusUpdated();

private slots:
    void _initFromTlog(const QJsonObject data);
    void _initStatus(const QJsonArray data);
    void _initStatus(const QJsonObject data);

private:
    bool    _isFemale;
    bool    _isPrivacy;
    bool    _isOnline;
    bool    _isFlow;
    bool    _isPremium;
    bool    _isDaylog;
    QString _title;
    QString _entriesCount;
    QString _publicEntriesCount;
    QString _privateEntriesCount;
    QString _daysCount;
    QString _followingsCount;
    QString _lastSeenAt;

    bool _loading;
};
