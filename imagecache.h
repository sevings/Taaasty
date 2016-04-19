#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QQuickItem>
#include <QStringList>
#include "cachemanager.h"



class ImageCache : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool readyToDownload READ isReadyToDownload NOTIFY readyToDownload)
    Q_PROPERTY(int kbytesReceived READ received NOTIFY receivedChanged)
    Q_PROPERTY(int kbytesTotal READ total NOTIFY totalChanged)
    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY downloadingChanged)
    Q_PROPERTY(QString extension READ getExtension NOTIFY extensionChanged)

public:
    explicit ImageCache(QQuickItem *parent = 0);

    QString source();
    void setSource(QString source);
    bool isReadyToDownload();
    int received();
    int total();
    bool isDownloading();
    QString getExtension();

public slots:
    void download();
    void abortDownload();

private slots:
    void imageAvailable();
    void changeBytes(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void available();
    void sourceChanged();
    void readyToDownload();
    void receivedChanged();
    void totalChanged();
    void downloadingChanged();
    void extensionChanged();

private:
    QString filename;
    QString sourceUrl;
    ImagePath* image;
    bool ready;
    int kbytesReceived;
    int kbytesTotal;
    QString extension;
    void changeExtension(QString url);
};

#endif // IMAGECACHE_H
