#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QDir>

#include "tasty.h"

class QNetworkAccessManager;
class QNetworkReply;



class ImagePath: public QObject
{
    Q_OBJECT
public:
    ImagePath(QNetworkAccessManager *web, QDir path, QString url);

    QString filename;
    bool isAvailable;

    void get();
    bool isDownloading();
    void abort();

signals:
    void available();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private slots:
    void _saveFile();
    void _emitProgressChanged(qint64 bytesReceived, qint64 bytesTotal);
    void _ignoreSslErrors();

private:
    bool _isExists();
    bool _checkExists();

    QNetworkReply* _reply;
    QNetworkAccessManager* _web;
    QString _url;
};



class CacheManager : public QObject
{
    Q_OBJECT
public:
    static CacheManager* instance();

    ImagePath* download(QString url);

private:
    CacheManager();
    CacheManager(const CacheManager& root);
    CacheManager& operator=(const CacheManager&);

    QNetworkAccessManager* _web;
    QDir _path;
};

#endif // CACHEMANAGER_H
