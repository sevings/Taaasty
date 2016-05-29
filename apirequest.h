#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>

#include "tasty.h"



class ApiRequest : public QObject
{
    Q_OBJECT
public:
    explicit ApiRequest(const QString url,
                        const bool accessTokenRequired = false,
                        const QNetworkAccessManager::Operation method = QNetworkAccessManager::GetOperation,
                        const QString data = QString());
    ~ApiRequest();

signals:
    void success(const QJsonObject data);
    void success(const QJsonArray data);
    void error(QNetworkReply::NetworkError code);
    void error(const int code, const QString text);

private slots:
    void _printNetworkError(QNetworkReply::NetworkError code);
    void _restart(QNetworkAccessManager::NetworkAccessibility na);
    void _finished();

private:
    void _start();

    QNetworkReply*  _reply;
    QNetworkRequest _request;
    QByteArray      _readyData;

    const QNetworkAccessManager::Operation _method;
};

#endif // APIREQUEST_H
