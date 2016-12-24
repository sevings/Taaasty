#ifndef UPLOADMODEL_H
#define UPLOADMODEL_H

#include <QStringListModel>
#include <QFutureWatcher>
#include <QImageReader>
#include <QHttpPart>

#include "../defines.h"



class UploadModel : public QStringListModel
{
    Q_OBJECT
public:
    UploadModel(QObject* parent = nullptr);
    ~UploadModel();

    bool isLoading() const;

    const QList<QHttpPart>& parts() const;

    void loadFiles();

signals:
    void loaded();

public slots:
    bool append(const QString& fileName);
    bool remove(int i);

    void load();
    void save();
    void clear();

private:
    void _loadFiles();

    QList<QHttpPart>             _parts;
    QMap<QString, QImageReader*> _readers;
    QFutureWatcher<void>         _watcher;
};

#endif // UPLOADMODEL_H
