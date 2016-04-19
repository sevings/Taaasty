#ifndef TASTY_H
#define TASTY_H

#include <QObject>
#include <QJsonObject>
#include <QNetworkAccessManager>

#include "settings.h"



class Tasty : public QObject
{
    Q_OBJECT

public:
    explicit Tasty(QObject *parent = 0);
    ~Tasty();

    static Tasty* instance();
    Settings* settings() const { return _settings; }
    QNetworkAccessManager* manager() const { return _manager; }

signals:
    void authorizationNeeded();
    void authorized();

public slots:
    void authorize(const QString login, const QString password);
    void postEntry(const QString title, const QString content);

    void getMe();

private slots:
    void _readAccessToken(const QJsonObject data);

    void _readMe(const QJsonObject data);

private:
    Settings* _settings;
    QNetworkAccessManager* _manager;
};

#endif // TASTY_H
