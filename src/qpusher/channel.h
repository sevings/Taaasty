#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>
#include <QSet>

class Pusher;



class Channel : public QObject
{
    Q_OBJECT

    friend class Pusher;

public:
    void subscribeToPrivate(const QString auth, const QString data = QString());

    bool isSubscribed() const;

    QString name() const;

    QSet<QString> presenceMemberIds() const;

public slots:
    void resubscribe();
    void unsubscribe();

signals:
    void authNeeded();

    void subscribed();
    void unsubscribed();

    void event(const QString event, const QString data);

    void memberAdded(const QString id);
    void memberRemoved(const QString id);

private:
    Channel(const QString name, bool isPublic, Pusher* pusher);

    bool _sendSubscription(const QString auth = QString(),
                           const QString channelData = QString());

    bool _sendUnsubscription();

    void _clear();

    bool _subscribed;
    bool _isPublic;

    QString _name;

    QSet<QString> _presenceMemberIds;

    Pusher* _pusher;
};

#endif // CHANNEL_H
