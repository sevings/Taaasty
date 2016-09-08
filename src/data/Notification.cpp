#include "Notification.h"

#include "User.h"

#include "Entry.h"

#include "../tasty.h"
#include "../pusherclient.h"
#include "../apirequest.h"



Notification::Notification(QObject* parent)
    : TastyData(parent)
    , _sender(new User(this))
    , _read(false)
    , _entityId(0)
    , _parentId(0)
{

}



Notification::Notification(const QJsonObject data, QObject *parent)
    : TastyData(parent)
{
    _id         = data.value("id").toInt();
    _createdAt  = Tasty::parseDate(data.value("created_at").toString());
    _sender     = new User(data.value("sender").toObject(), this);
    _read       = !data.value("read_at").isNull();
    _action     = data.value("action").toString();
    _actionText = data.value("action_text").toString();
    _text       = data.value("text").toString();
    _entityId   = data.value("entity_id").toInt();

    auto entityType = data.value("entity_type").toString();
    if (entityType == "Entry")
        _entityType = EntryType;
    else if (entityType == "Relationship")
        _entityType = RelationshipType;
    else if (entityType == "Comment")
        _entityType = CommentType;
    else
    {
        qDebug() << "Unknown notification entity type:" << entityType;
        _entityType = UnknownType;
    }

    _parentId   = data.value("parent_id").toInt();
    _parentType = data.value("parent_type").toString();

    Tasty::instance()->pusher()->addNotification(this);

    emit idChanged();
}



Notification::~Notification()
{
    Tasty::instance()->pusher()->removeNotification(_id);
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



bool Notification::isRead() const
{
    return _read;
}



int Notification::parentId() const
{
    return _parentId;
}



Entry* Notification::entry()
{
    if (_entry )
        return _entry.data();

    int id = 0;
    switch (_entityType)
    {
    case EntryType:
        id = _entityId;
        break;
    case CommentType:
        id = _parentId;
        break;
    default:
        break;
    }

    if (id <= 0)
        return nullptr;

    _entry = Tasty::instance()->pusher()->entry(id);
    if (_entry)
        return _entry.data();

    _entry = EntryPtr::create((QObject*)nullptr);
    _entry->setId(id);
    return _entry.data();
}



void Notification::read()
{
    if (_read || _id <= 0 || isLoading())
        return;

    auto url = QString("v2/messenger/notifications/%1/read.json").arg(_id);
    _request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_updateRead(QJsonObject))));
    
    _initRequest();
}



void Notification::_updateRead(const QJsonObject data)
{
    if (_read || data.value("id").toInt() != _id)
        return;

    _read = !data.value("read_at").isNull();
    emit readChanged();
}
