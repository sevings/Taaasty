#ifndef PUSHERCLIENT_H
#define PUSHERCLIENT_H

#include <QObject>
#include <QJsonObject>

class Pusher;
class Tasty;



class PusherClient : public QObject
{
    Q_OBJECT
public:
    explicit PusherClient(Tasty* tasty = nullptr);

signals:
    void notification(const QJsonObject data);
    void unreadChats(int count);

public slots:

private slots:
    void _getPusherAuth();
    void _resubscribeToPrivate();
    void _subscribeToPrivate(const QJsonObject data);
    void _handlePrivatePusherEvent(const QString event, const QString data);

private:
    void _addPrivateChannel();

    Pusher* _pusher;
    QString _privateChannel;

    Tasty* _tasty;
};

#endif // PUSHERCLIENT_H
