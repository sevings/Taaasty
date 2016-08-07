#ifndef FLOW_H
#define FLOW_H

#include <QObject>
#include <QJsonObject>



class Flow : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int     id             MEMBER  _id             NOTIFY updated)
    Q_PROPERTY(QString name           MEMBER  _name           NOTIFY updated)
    Q_PROPERTY(QString slug           MEMBER  _slug           NOTIFY updated)
    Q_PROPERTY(QString title          MEMBER  _title          NOTIFY updated)
    Q_PROPERTY(QString url            MEMBER  _url            NOTIFY updated)
    Q_PROPERTY(QString picUrl         MEMBER  _pic            NOTIFY updated)
    Q_PROPERTY(bool    isPrivate      MEMBER  _isPrivate      NOTIFY updated)
    Q_PROPERTY(bool    isPremoderate  MEMBER  _isPremoderate  NOTIFY updated)
    Q_PROPERTY(QString followersCount MEMBER  _followersCount NOTIFY updated)
    Q_PROPERTY(QString entriesCount   MEMBER  _entriesCount   NOTIFY updated)

public:
    explicit Flow(QObject* parent = nullptr);
    Flow(const QJsonObject data, QObject* parent = nullptr);

    Q_INVOKABLE int  flowId() const { return _id; }
    Q_INVOKABLE void setId(const int id);

signals:
    void updated();
    void loadingChanged();

private slots:
    void _init(const QJsonObject data);

private:
    int     _id;
    QString _name;
    QString _slug;
    QString _title;
    QString _url;
    QString _pic;
    bool    _isPrivate;
    bool    _isPremoderate;
    QString _followersCount;
    QString _entriesCount;

    bool _loading;
};

#endif // FLOW_H
