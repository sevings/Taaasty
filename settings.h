#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDateTime>



class Settings : public QObject
{
    Q_OBJECT

//    Q_PROPERTY(QString login READ login WRITE setLogin)
//    Q_PROPERTY(QString password READ password WRITE setPassword)
//    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken)

public:
    explicit Settings(QObject *parent = 0);

    QString login() const;
    void setLogin(const QString login);

    QString accessToken() const;
    void setAccessToken(const QString token);

    QDateTime expiresAt() const;
    void setExpiresAt(const QString date);

signals:

public slots:

private:
    QSettings _settings;

};

#endif // SETTINGS_H
