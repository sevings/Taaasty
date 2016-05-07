#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QQuickItem>

class ImagePath;



class ImageCache : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString  source          READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool     readyToDownload READ isReadyToDownload      NOTIFY readyToDownload)
    Q_PROPERTY(int      kbytesReceived  READ received               NOTIFY receivedChanged)
    Q_PROPERTY(int      kbytesTotal     READ total                  NOTIFY totalChanged)
    Q_PROPERTY(bool     isDownloading   READ isDownloading          NOTIFY downloadingChanged)
    Q_PROPERTY(QString  extension       READ getExtension           NOTIFY extensionChanged)

public:
    explicit ImageCache(QQuickItem *parent = 0);

    QString source() const;
    void    setSource(QString source);

    bool    isReadyToDownload() const;
    int     received() const;
    int     total() const;
    bool    isDownloading() const;
    QString getExtension() const;

public slots:
    void download();
    void abortDownload();

private slots:
    void _imageAvailable();
    void _changeBytes(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void available();
    void sourceChanged();
    void readyToDownload();
    void receivedChanged();
    void totalChanged();
    void downloadingChanged();
    void extensionChanged();

private:
    QString _filename;
    QString _sourceUrl;
    ImagePath* image;

//    bool _ready;
    int     _kbytesReceived;
    int     _kbytesTotal;
    QString _extension;

    void _changeExtension(QString url);
};

#endif // IMAGECACHE_H
