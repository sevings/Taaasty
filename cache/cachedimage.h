#ifndef CACHEDIMAGE_H
#define CACHEDIMAGE_H

#include <QObject>

class CacheManager;
class QNetworkReply;


class CachedImage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  source              READ source            NOTIFY available)
    Q_PROPERTY(bool     isReadyToDownload   READ isReadyToDownload NOTIFY readyToDownload)
    Q_PROPERTY(int      kbytesReceived      READ received          NOTIFY receivedChanged)
    Q_PROPERTY(int      kbytesTotal         READ total             NOTIFY totalChanged)
    Q_PROPERTY(bool     isDownloading       READ isDownloading     NOTIFY downloadingChanged)
    Q_PROPERTY(QString  format              READ format            NOTIFY formatChanged)

public:
    enum ImageFormat { UnknownFormat, GifFormat, JpegFormat, PngFormat };
    Q_ENUMS(ImageFormat)

    explicit CachedImage(CacheManager* parent = nullptr, QString url = QString());

    Q_INVOKABLE QString source() const;

    Q_INVOKABLE int         received() const    { return _kbytesReceived; }
    Q_INVOKABLE int         total() const       { return _kbytesTotal; }

    Q_INVOKABLE bool        isReadyToDownload() const;
    Q_INVOKABLE bool        isDownloading() const;

    Q_INVOKABLE ImageFormat format() const      { return _format; }

public slots:
    void getInfo();
    void download();
    void abortDownload();

private slots:
    void _setProperties();
    void _saveFile();
    void _changeBytes(qint64 bytesReceived, qint64 bytesTotal);

signals:
    void available();
    void readyToDownload();
    void receivedChanged();
    void totalChanged();
    void downloadingChanged();
    void formatChanged();

private:
    CacheManager*  _man;
    QNetworkReply* _headReply;
    QNetworkReply* _reply;

    ImageFormat _format;

    QString _hash;
    QString _url;
    QString _extension;

    int     _kbytesReceived;
    int     _kbytesTotal;

    bool _exists();
    void _setFormat(const QString format);
};

#endif // CACHEDIMAGE_H
