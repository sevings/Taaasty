#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QDir>

#include "tasty.h"



class ImagePath: public QObject
{
    Q_OBJECT
public:
    ImagePath(QNetworkAccessManager *web, QDir path, QString url);
    void get();
    QString filename;
    bool isAvailable;
    bool isDownloading();
    void abort();

signals:
    void available();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void downloadFinished();
    void downloadProgressChanged(qint64 bytesReceived, qint64 bytesTotal);
    void ignoreSslErrors();

private:
    bool exists();
    bool checkExists();
    QNetworkReply* reply;
    QNetworkAccessManager* web;
    QString url;
};

class CacheManager : public QObject
{
    Q_OBJECT
public:
    static CacheManager* Instance();
    ImagePath* download(QString url);

private:
    CacheManager();
    CacheManager(const CacheManager& root);
    CacheManager& operator=(const CacheManager&);

    QNetworkAccessManager* web;
    QDir path;
};

#endif // CACHEMANAGER_H
