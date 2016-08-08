#ifndef NOTIFICATIONSMODEL_H
#define NOTIFICATIONSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>

class Notification;

#ifdef Q_OS_ANDROID
class AndroidNotifier;
#endif


class NotificationsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)
    Q_PROPERTY(bool unread  READ unread  NOTIFY unreadChanged)

public:
    NotificationsModel(QObject* parent = nullptr);
    ~NotificationsModel();

    static NotificationsModel* instance(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE bool hasMore() const { return canFetchMore(QModelIndex()); }
    Q_INVOKABLE bool unread() const;
    Q_INVOKABLE void markAsRead();

signals:
    void hasMoreChanged();
    void unreadChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    // void _readSuccess();

    void _addItems(QJsonObject data);
    void _addPush(QJsonObject data);

    void _reloadAll();

    void _setNotLoading();

private:
    QList<Notification*> _notifs;
    QString _url;
    bool _loading;
    int _totalCount;

#ifdef Q_OS_ANDROID
    AndroidNotifier* _androidNotifier;
#endif
};

#endif // NOTIFICATIONSMODEL_H
