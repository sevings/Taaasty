#ifndef PUSHER_H
#define PUSHER_H

#include <QObject>
#include <QHash>
#include <QUrl>

class Channel;
class QWebSocket;
class QTimer;



class Pusher : public QObject
{
    Q_OBJECT

    friend class Channel;

public:
    Pusher(const QString appKey, QObject* parent = nullptr);
    ~Pusher();

    bool isConnected() const;
    QString socketId() const;

    Channel* channel(const QString name) const;

    Channel* subscribe(const QString channelName, bool isPublic = true);
    void   unsubscribe(const QString channelName);

signals:
    void connected();
    void disconnected(int code, const QString reason);

    void errorEvent(const int code, const QString message);

public slots:
    void connect();
    void disconnect();
    void reconnect();

private slots:
    void _handleEvent(const QString& message);
    void _emitDisconnected();

private:
    bool _send(const QString channel, const QString event, const QString data);
    bool _send(const QJsonObject json);

    QUrl        _url;
    QString     _socketId;
    QWebSocket* _socket;
    bool        _imReconnect;

    QHash<QString, Channel*> _channels;

    QTimer* _pingTimer;
    QTimer* _pongTimer;
    QTimer* _reconnectTimer;
};

#endif // PUSHER_H
