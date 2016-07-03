#include "Notification.h"

#include "../defines.h"

#include "User.h"
#include "../tasty.h"


Notification::Notification(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _sender(new User(this))
    , _read(false)
    , _entityId(0)
    , _parentId(0)
{

}

Notification::Notification(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _id         = data.value("id").toInt();
    _createdAt  = Tasty::parseDate(data.value("created_at").toString());
    _sender     = new User(data.value("sender").toObject(), this);
    _read       = !data.value("read_at").isNull();
    _action     = data.value("action").toString();
    _actionText = data.value("action_text").toString();
    _text       = data.value("text").toString();
    _entityId   = data.value("entity_id").toInt();
    _entityType = data.value("entity_type").toString();
    _parentId   = data.value("parent_id").toInt();
    _parentType = data.value("parent_type").toString();
}



int Notification::entityId() const
{
    return _entityId;
}



User* Notification::sender() const
{
    return _sender;
}



QString Notification::actionText() const
{
    return _actionText;
}



QString Notification::text() const
{
    return _text;
}
