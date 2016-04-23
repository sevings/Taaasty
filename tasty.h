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

    Q_PROPERTY(int busy READ busy NOTIFY busyChanged)

public:
    explicit Tasty(QObject *parent = 0);
    ~Tasty();

    static Tasty* instance();
    Settings* settings() const { return _settings; }
    QNetworkAccessManager* manager() const { return _manager; }

    void incBusy();
    void decBusy();

signals:
    void busyChanged();

    void authorizationNeeded();
    void authorized();

    void ratingChanged(const QJsonObject rating);

public slots:
    int busy() const { return _busy; }

    void authorize(const QString login, const QString password);
    void postEntry(const QString title, const QString content);

    void getMe();

    void addComment(const int entryId, const QString text);
    void editComment(const int id, const QString text);
    void removeComment(const int id);

private slots:
    void _readAccessToken(const QJsonObject data);

    void _readComment(const QJsonObject data);

    void _readMe(const QJsonObject data);

private:
    Settings* _settings;
    QNetworkAccessManager* _manager;
    int _busy;
};

#endif // TASTY_H
