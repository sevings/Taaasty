#include "Flow.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"



Flow::Flow(QObject *parent)
    : TastyData(parent)
    , _isPrivate(false)
    , _isPremoderate(false)
{

}



Flow::Flow(const QJsonObject data, QObject* parent)
    : TastyData(parent)
{
    _init(data);
}



void Flow::setId(const int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    _request = new ApiRequest(QString("v1/flows/%1.json").arg(_id));
    connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));

    _initRequest();
}



void Flow::_init(const QJsonObject data)
{
    _id             = data.value("id").toInt();
    _name           = data.value("name").toString();
    _slug           = data.value("slug").toString();
    _title          = data.value("title").toString();
    _url            = data.value("tlog_url").toString();
    _pic            = data.value("flowpic").toObject().value("original_url").toString();
    _isPrivate      = data.value("is_privacy").toBool();
    _isPremoderate  = data.value("is_premoderate").toBool();
    _followersCount = Tasty::num2str(data.value("followers_count").toInt(),
                                     "подписчик", "подписчика", "подписчиков");
    _entriesCount   = Tasty::num2str(data.value("public_tlog_entries_count").toInt(),
                                     "запись", "записи", "записей");

    emit idChanged();
    emit updated();
}
