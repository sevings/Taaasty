#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QObject>
#include <QHash>

class QNetworkAccessManager;
class CachedImage;



class CacheManager : public QObject
{
    Q_OBJECT

public:
    static CacheManager* instance(QNetworkAccessManager* web = nullptr);

    Q_INVOKABLE CachedImage* image(QString url);

    QNetworkAccessManager* web() const;
    QString path() const;

    int  maxWidth() const;
    void setMaxWidth(int maxWidth);

    bool autoload() const;
    void setAutoload(bool autoload);

private:
    CacheManager(QNetworkAccessManager* web = nullptr);

    QHash<QString, CachedImage*> _images;
    QNetworkAccessManager* _web;
    QString _path;
    int _maxWidth;
    bool _autoload;
};

#endif // CACHEMANAGER_H
