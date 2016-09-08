#pragma once

#include <QObject>
#include <QJsonObject>

#include "TastyData.h"

class ApiRequest;



class User: public TastyData
{
    Q_OBJECT

    Q_PROPERTY(QString tlogUrl      MEMBER _tlogUrl     NOTIFY updated)
    Q_PROPERTY(QString name         MEMBER _name        NOTIFY updated)
    Q_PROPERTY(QString slug         MEMBER _slug        NOTIFY updated)

    Q_PROPERTY(QString originalPic     MEMBER _originalPic     NOTIFY updated)
    Q_PROPERTY(QString largePic        MEMBER _largePic        NOTIFY updated)
    Q_PROPERTY(QString thumb128        MEMBER _thumb128        NOTIFY updated)
    Q_PROPERTY(QString thumb64         MEMBER _thumb64         NOTIFY updated)
    Q_PROPERTY(QString symbol          MEMBER _symbol          NOTIFY updated)
    Q_PROPERTY(QString backgroundColor MEMBER _backgroundColor NOTIFY updated)
    Q_PROPERTY(QString nameColor       MEMBER _nameColor       NOTIFY updated)

public:
    User(QObject* parent = nullptr);
    User(const QJsonObject data, QObject* parent = nullptr);

    void    setId(int id);

    QString name() const;
    QString slug() const;

    User& operator=(const User& other);

signals:
    void updated();

protected slots:
    void _init(const QJsonObject data);
    void _initFromTlog(const QJsonObject data);

private:
    QString _tlogUrl;
    QString _name;
    QString _slug;

    QString _originalPic;
    QString _largePic;
    QString _thumb128;
    QString _thumb64;
    QString _symbol;
    QString _backgroundColor;
    QString _nameColor;
};
