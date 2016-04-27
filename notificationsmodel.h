#ifndef NOTIFICATIONSMODEL_H
#define NOTIFICATIONSMODEL_H

#include <QObject>
#include <QDateTime>
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>

class Notification;



class NotificationsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)

public:
    NotificationsModel(QObject* parent = nullptr);
    ~NotificationsModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE bool hasMore() const { return canFetchMore(QModelIndex()); }
    Q_INVOKABLE void markAsRead();
    
public slots:
    void check();
    
signals:
    void hasMoreChanged();
    
private slots:
    void _readSuccess(QJsonObject data);

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _addNewest(QJsonObject data);

private:
    QList<Notification*> _notifs;
    QString _url;
    bool _loading;
    int _totalCount;
};

#endif // NOTIFICATIONSMODEL_H
