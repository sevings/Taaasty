#ifndef TASTY_H
#define TASTY_H

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>

#include "settings.h"

class ApiRequest;
class PusherClient;
class User;



class Tasty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isAuthorized        READ isAuthorized        NOTIFY authorized)
    Q_PROPERTY(int  unreadChats         READ unreadChats         NOTIFY unreadChatsChanged)
    Q_PROPERTY(int  unreadNotifications READ unreadNotifications NOTIFY unreadNotificationsChanged)

public:
    explicit Tasty(QNetworkAccessManager* web = nullptr);
    ~Tasty();

    static Tasty* instance(QNetworkAccessManager* web = nullptr);

    Settings*               settings() const { return _settings; }
    QNetworkAccessManager*  manager() const { return _manager; }
    PusherClient*           pusher() const { return _pusher; }

    Q_INVOKABLE bool isAuthorized() const;

    Q_INVOKABLE int unreadChats()         const { return _unreadChats; }
    Q_INVOKABLE int unreadNotifications() const { return _unreadNotifications; }

    Q_INVOKABLE static QString num2str(const int n, const QString str1,
                    const QString str234, const QString str5);
    static QString parseDate(const QString d, const bool bigLetter = true);
    static void correctHtml(QString& html, bool isEntry = true);
    static QString truncateHtml(QString html, int length = 300);

    Q_INVOKABLE void setImageWidth(int entry, int comment);

    User* me();

signals:
    void authorizationNeeded();
    void authorized();

    void unreadChatsChanged();
    void unreadNotificationsChanged();

    void error(const int code, const QString text);
    void info(const QString text);

    void htmlRecorrectionNeeded();

    void networkAccessible();

public slots:
    void authorize(const QString login, const QString password);
    void reconnectToPusher();

private slots:
    void _readAccessToken(const QJsonObject data);

    void _showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible);

    void _setUnreadChats(int count);
    void _setUnreadNotifications(int count);

    void _saveOrReconnect(Qt::ApplicationState state);

private:
    Settings* _settings;
    QNetworkAccessManager* _manager;
    PusherClient* _pusher;

    int _entryImageWidth;
    int _commentImageWidth;

    int _unreadChats;
    int _unreadNotifications;

    User* _me;
};

#endif // TASTY_H
