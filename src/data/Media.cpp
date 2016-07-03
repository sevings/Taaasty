#include "Media.h"

#include <QJsonArray>

#include "../defines.h"

#include "AttachedImage.h"



Media::Media(QObject* parent)
    : QObject(parent)
    , _duration(0)
    , _thumbnail(new AttachedImage(this))
    , _icon(new AttachedImage(this))
{

}



Media::Media(const QJsonObject data, QObject* parent)
    : QObject(parent)
{
    auto meta = data.value("meta").toObject();
    _title    = meta.value("title").toString();
    _duration = meta.value("duration").toInt();

    auto links = data.value("links").toObject();
    _url = links.value("player").toArray().first().toObject().value("href").toString();

    auto thumb = links.value("thumbnail").toArray().first().toObject();
    _thumbnail = new AttachedImage(thumb, this);

    auto icon = links.value("icon").toArray().first().toObject();
    _icon     = new AttachedImage(icon, this);
}
