#include "User.h"

#include "../defines.h"

#include "../apirequest.h"



User::User(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _loading(false)
{

}



User::User(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _loading(false)
{
    _init(data);
}



int User::id() const
{
    return _id;
}



void User::setId(int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;

    auto request = new ApiRequest(QString("v1/tlog/%1.json").arg(_id));
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_initFromTlog(QJsonObject)));

    _loading = true;
}



QString User::name() const
{
    return _name;
}



QString User::slug() const
{
    return _slug;
}



void User::_init(const QJsonObject data)
{
    _id         = data.value("id").toInt();
    _tlogUrl    = data.value("tlog_url").toString();
    _name       = data.value("name").toString();
    _slug       = data.value("slug").toString();

    auto userpic = data.value("userpic").toObject();

    _originalPic    = userpic.value("original_url").toString();
    _largePic       = userpic.value("large_url").toString();
    _thumb128       = userpic.value("thumb128_url").toString();
    _thumb64        = userpic.value("thumb64_url").toString();
    _symbol         = userpic.value("symbol").toString();

    auto colors = userpic.value("default_colors").toObject();

    _backgroundColor = colors.value("background").toString();
    _nameColor       = colors.value("name").toString();

    emit updated();
}



void User::_initFromTlog(const QJsonObject data)
{
    auto author = data.value("author").toObject();
    _init(author);
}
