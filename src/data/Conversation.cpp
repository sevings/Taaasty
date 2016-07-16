#include "Conversation.h"

#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"

#include "Entry.h"
#include "Message.h"
#include "User.h"
#include "Author.h"



Conversation::Conversation(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _type(UninitializedConversation)
    , _unreadCount(0)
    , _unreceivedCount(0)
    , _totalCount(0)
    , _userId(0)
    , _recipientId(0)
    , _isDisabled(false)
    , _notDisturb(false)
    , _isAnonymous(false)
    , _lastMessage(nullptr)
    , _entry(nullptr)
    , _recipient(nullptr)
    , _loading(false)
{
    
}



Conversation::Conversation(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _lastMessage(nullptr)
    , _entry(nullptr)
    , _recipient(nullptr)
    , _loading(false)
{
    _init(data);
}



int Conversation::id() const
{
    return _id;
}



void Conversation::setId(int id)
{
    if (_loading || id <= 0 || id == _id)
        return;

    _id = id;

    auto request = new ApiRequest(QString("/v2/messenger/conversations/by_id/%1.json").arg(_id), true);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));

    _loading = true;
    emit loadingChanged();
}



void Conversation::setUserId(int id)
{
    if (_loading || id <= 0 || id == _userId)
        return;

    _userId = id;

    auto request = new ApiRequest(QString("v2/messenger/conversations/by_user_id/%1.json").arg(_userId), true, QNetworkAccessManager::PostOperation);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));

    _loading = true;
    emit loadingChanged();
}



void Conversation::setSlug(const QString slug)
{
    if (_loading || slug.isEmpty())
        return;

    auto request = new ApiRequest(QString("v2/messenger/conversations/by_slug/%1.json").arg(slug), true, QNetworkAccessManager::PostOperation);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));

    _loading = true;
    emit loadingChanged();
}



void Conversation::setEntryId(int entryId)
{
    if (_loading || entryId <= 0 || (_entry && _entry->entryId() == entryId))
        return;

    auto data = QString("id=%1").arg(entryId);
    auto request = new ApiRequest(QString("v2/messenger/conversations/by_entry_id.json"), true, QNetworkAccessManager::PostOperation, data);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));

    _loading = true;
    emit loadingChanged();
}



void Conversation::_init(const QJsonObject data)
{
     _id                = data.value("id").toInt();
     
     auto type = data.value("type").toString();
     if (type == "PublicConversation")
         _type = PublicConversation;
     else if (type == "PrivateConversation")
         _type = PrivateConversation;
     else
     {
         qDebug() << "Unsupported conversation type:" << type;
         _type = UninitializedConversation;
     }
     
     _unreadCount       = data.value("unread_messages_count").toInt();
     _unreceivedCount   = data.value("unreceived_messages_count").toInt();
     _totalCount        = data.value("messages_count").toInt();
     _userId            = data.value("user_id").toInt();
     _recipientId       = data.value("recipient_id").toInt();
     _isDisabled        = data.value("is_disabled").toBool();
     _notDisturb        = data.value("not_disturb").toBool();
     _isAnonymous       = data.value("is_anonymous").toBool();

     delete _lastMessage;
     _lastMessage       = new Message(data.value("last_message").toObject(), this);

     delete _entry;
     _entry             = data.contains("entry") ? new Entry(data.value("entry").toObject(), this) : nullptr;

     delete _recipient;
     if (data.contains("recipient"))
     {
         _recipient     =  new Author(data.value("recipient").toObject(), this);
         _recipient->reload();
     }
     else
         _recipient = nullptr;
     
     qDeleteAll(_users);
     _users.clear();
     auto users = data.value("users").toArray();
     foreach(auto userData, users)
        _users << new User(userData.toObject(), this); // TODO: isOnline
             
     qDeleteAll(_deletedUsers);
     _deletedUsers.clear();
     users = data.value("users_deleted").toArray();
     foreach(auto userData, users)
        _deletedUsers << new User(userData.toObject(), this);
        
     emit updated();
}



void Conversation::_setNotLoading()
{
    _loading = false;
    emit loadingChanged();
}

int Conversation::totalCount() const
{
    return _totalCount;
}
