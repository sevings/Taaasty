#ifndef APIREQUEST_H
#define APIREQUEST_H

#include <QNetworkReply>
#include <QJsonObject>

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
    void error(QNetworkReply::NetworkError code);

private slots:
    void _printNetworkError(QNetworkReply::NetworkError code);
    void _finished();

private:
    QNetworkReply* _reply;
};

#endif // APIREQUEST_H
