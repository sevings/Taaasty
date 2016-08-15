#pragma once

#include <QObject>
#include <QJsonObject>

class Media;
class Message;



class AttachedImage: public QObject
{
    Q_OBJECT

    friend class AttachedImagesModel;

    Q_PROPERTY(QString url    MEMBER _url    CONSTANT)
    Q_PROPERTY(int     height MEMBER _height CONSTANT)
    Q_PROPERTY(int     width  MEMBER _width  CONSTANT)
    Q_PROPERTY(QString type   MEMBER _type   CONSTANT)

public:
    AttachedImage(QObject* parent = nullptr);
    AttachedImage(const QJsonObject data, QObject* parent = nullptr);
    AttachedImage(const QJsonObject data, Media* parent);

private:
    QString _url;
    QString _type;
    int     _width;
    int     _height;
};
