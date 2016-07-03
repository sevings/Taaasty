#pragma once

#include <QObject>
#include <QJsonObject>



class User: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int     id           MEMBER _id          NOTIFY updated)
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
    User(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    int     id() const;
    void    setId(int id);

    QString name() const;
    QString slug() const;

signals:
    void updated();

protected slots:
    void _init(const QJsonObject data);
    void _initFromTlog(const QJsonObject data);

private:
    int     _id;
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

    bool _loading;
};