#include "uploadmodel.h"

#include <QFileInfo>
#include <QtConcurrent>
#include <QUrl>

#include "../tasty.h"
#include "../settings.h"

#define TASTY_IMAGE_WIDTH 712



UploadModel::UploadModel(QObject* parent)
    : QStringListModel(parent)
{
    load();

    Q_TEST(connect(&_watcher, &QFutureWatcher<void>::finished, this, &UploadModel::loaded));
}



UploadModel::~UploadModel()
{
    save();

    qDeleteAll(_readers);
}



bool UploadModel::isLoading() const
{
    return _watcher.isRunning();
}



const QList<QHttpPart>& UploadModel::parts() const
{
    Q_ASSERT(!isLoading());

    return _parts;
}



void UploadModel::loadFiles()
{
    if (!rowCount())
    {
        emit loaded();
        return;
    }

    auto future = QtConcurrent::run(this, &UploadModel::_loadFiles);
    _watcher.setFuture(future);
}



bool UploadModel::append(const QString& fileName)
{
    if (isLoading())
        return false;

    QScopedPointer<QImageReader> reader(new QImageReader(fileName));
    if (!reader->canRead())
        return false;

    if (!insertRow(rowCount()))
        return false;

    if (!setData(index(rowCount() - 1), QUrl::fromLocalFile(fileName), Qt::DisplayRole))
        return false;

    _readers.insert(fileName, reader.take());

    return true;
}



bool UploadModel::remove(int i)
{
    if (isLoading())
        return false;

    auto fileName = QUrl(data(index(i), Qt::DisplayRole).toString()).toLocalFile();
    delete _readers.take(fileName);

    return removeRow(i);
}



void UploadModel::load()
{
    auto images = pTasty->settings()->lastImages();
    foreach (auto image, images)
        append(image);
}



void UploadModel::save()
{
    pTasty->settings()->setLastImages(_readers.keys());
}



void UploadModel::clear()
{
    if (isLoading())
        return;

    _parts.clear();

    qDeleteAll(_readers);
    _readers.clear();

    setStringList(QStringList());
}



void UploadModel::_loadFiles()
{
    for (auto it = _readers.cbegin(); it != _readers.cend(); ++it)
    {
        auto fileName = QFileInfo(it.key()).fileName();
        auto reader = it.value();
        auto format = reader->format();

        QHttpPart imagePart;
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
                            QString("image/").append(QString::fromLatin1(format)));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant(QString("form-data; name=\"files[]\"; filename=\"%1\"").arg(fileName)));

        if (format == "gif" || reader->size().width() <= TASTY_IMAGE_WIDTH)
            imagePart.setBodyDevice(reader->device());
        else
        {
            auto image = reader->read();
            image = image.scaledToWidth(TASTY_IMAGE_WIDTH, Qt::SmoothTransformation);

            QByteArray body;
            QBuffer buffer(&body);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, format.constData());
            imagePart.setBody(body);
        }

        _parts << imagePart;
    }
}
