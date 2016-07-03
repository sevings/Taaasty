#include "AttachedImage.h"

#include "../defines.h"

#include "Media.h"



AttachedImage::AttachedImage(QObject* parent)
    : QObject(parent)
    , _width(0)
    , _height(0)
{

}



AttachedImage::AttachedImage(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    auto image = data.value("image").toObject();
    _url = image.value("url").toString();

    auto type = data.value("content_type").toString();
    _type = type.split("/").last();

    auto geometry = image.value("geometry").toObject();
    _width = geometry.value("width").toInt();
    _height = geometry.value("height").toInt();
}



AttachedImage::AttachedImage(const QJsonObject data, Media* parent)
    : QObject(parent)
{
    _url = data.value("href").toString();

    auto type = data.value("type").toString();
    _type = type.split("/").last();

    auto geometry = data.value("media").toObject();
    _width = geometry.value("width").toInt();
    _height = geometry.value("height").toInt();
}
