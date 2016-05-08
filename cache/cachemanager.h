#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QHash>

class QNetworkAccessManager;
class CachedImage;



class CacheManager : public QObject
{
    Q_OBJECT

    friend class CachedImage;

public:
    static CacheManager* instance(QNetworkAccessManager* web = nullptr);

    Q_INVOKABLE CachedImage* image(QString url);

private:
    CacheManager(QNetworkAccessManager* web = nullptr);

    QHash<QString, CachedImage*> _images;
    QNetworkAccessManager* _web;
    QString _path;
};

#endif // CACHEMANAGER_H
