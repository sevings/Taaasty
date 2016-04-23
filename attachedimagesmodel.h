#ifndef ATTACHEDIMAGESMODEL_H
#define ATTACHEDIMAGESMODEL_H

#include <QAbstractListModel>

class QJsonArray;
class AttachedImage;



class AttachedImagesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    AttachedImagesModel(const QJsonArray* data = nullptr, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE AttachedImage* first() const;
    Q_INVOKABLE double listRatio() const;

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<AttachedImage*> _images;
};

#endif // ATTACHEDIMAGESMODEL_H
