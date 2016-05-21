#ifndef TASTY_H
#define TASTY_H

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>

#include "settings.h"

class ApiRequest;



class Tasty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int  busy         READ busy         NOTIFY busyChanged)
    Q_PROPERTY(bool isAuthorized READ isAuthorized NOTIFY authorized)

public:
    explicit Tasty(QObject *parent = 0);
    ~Tasty();

    static Tasty* instance();
    Settings* settings() const { return _settings; }
    QNetworkAccessManager* manager() const { return _manager; }

    void incBusy();
    void decBusy();

    Q_INVOKABLE int busy() const { return _busy; }

    Q_INVOKABLE bool isAuthorized() const;

    Q_INVOKABLE static QString num2str(const int n, const QString str1,
                    const QString str234, const QString str5);
    static QString parseDate(const QString d);

signals:
    void busyChanged();

    void authorizationNeeded();
    void authorized();

    void ratingChanged(const QJsonObject data);

    void error(const int code, const QString text);

public slots:
    void authorize(const QString login, const QString password);
    void postEntry(const QString title, const QString content);

    void getMe();

private slots:
    void _readAccessToken(const QJsonObject data);

    void _showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible);

    void _readMe(const QJsonObject data);

private:
    Settings* _settings;
    QNetworkAccessManager* _manager;
    int _busy;
};

#endif // TASTY_H
