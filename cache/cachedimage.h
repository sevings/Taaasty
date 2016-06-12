#ifndef CACHEDIMAGE_H
#define CACHEDIMAGE_H

#include <QObject>
#include <QNetworkReply>
#include <QSslError>

class CacheManager;


class CachedImage : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString      source              READ source                         NOTIFY available)
    Q_PROPERTY(bool         isReadyToDownload   READ isReadyToDownload              NOTIFY readyToDownload)
    Q_PROPERTY(int          kbytesReceived      READ received                       NOTIFY receivedChanged)
    Q_PROPERTY(int          kbytesTotal         READ total                          NOTIFY totalChanged)
    Q_PROPERTY(bool         isDownloading       READ isDownloading                  NOTIFY downloadingChanged)
    Q_PROPERTY(bool         available           READ isAvailable                    NOTIFY available)
    Q_PROPERTY(QString      extension           READ extension   WRITE setExtension NOTIFY extensionChanged)

public:
    enum ImageFormat { UnknownFormat, GifFormat, JpegFormat, PngFormat };
    Q_ENUMS(ImageFormat)

    explicit CachedImage(CacheManager* parent = nullptr, QString url = QString());

    Q_INVOKABLE QString source() const;

    Q_INVOKABLE int     received() const    { return _kbytesReceived; }
    Q_INVOKABLE int     total() const       { return _kbytesTotal; }

    Q_INVOKABLE bool    isReadyToDownload() const;
    Q_INVOKABLE bool    isDownloading() const;
    Q_INVOKABLE bool    isAvailable() const { return _available; }

    Q_INVOKABLE QString extension() const   { return _extension; }
    Q_INVOKABLE void    setExtension(QString format);

public slots:
    void getInfo();
    void download();
    void abortDownload();
    void saveToFile(const QString filename);

private slots:
    void _setProperties();
    void _saveData();
    void _changeBytes(qint64 bytesReceived, qint64 bytesTotal);
    void _printError(QNetworkReply::NetworkError code);
    void _printErrors(const QList<QSslError>& errors);

signals:
    void available();
    void readyToDownload();
    void receivedChanged();
    void totalChanged();
    void downloadingChanged();
    void extensionChanged();

    void savingError();
    void fileSaved();

private:
    CacheManager*  _man;
    QNetworkReply* _headReply;
    QNetworkReply* _reply;
    QByteArray     _data;

    ImageFormat _format;

    QString _hash;
    QString _url;
    QString _extension;

    int     _kbytesReceived;
    int     _kbytesTotal;
    bool    _available;

    bool _exists();
    QString _path() const;
    void _saveFile();
};

#endif // CACHEDIMAGE_H
