#include "Conversation.h"

#include <QUuid>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"
#include "../pusherclient.h"

#include "Entry.h"
#include "Message.h"
#include "User.h"
#include "Author.h"

#include "../models/messagesmodel.h"



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
    , _entry(nullptr)
    , _recipient(nullptr)
    , _messages(nullptr)
    , _loading(false)
{
    
}



Conversation::Conversation(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entry(nullptr)
    , _recipient(nullptr)
    , _messages(nullptr)
    , _loading(false)
{
    _init(data);
}



Conversation::~Conversation()
{
    Tasty::instance()->pusher()->removeChat(_id);
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
     else if (type == "GroupConversation")
         _type = GroupConversation;
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

     if (!_entry && data.contains("entry"))
        _entry          =  new Entry(data.value("entry").toObject(), this);

     delete _recipient;
     if (data.contains("recipient"))
     {
         _recipient     =  new Author(data.value("recipient").toObject(), this);
         _recipient->reload();
     }
     else
         _recipient     = nullptr;

     if (data.contains("topic"))
         _topic         = data.value("topic").toString();
     else if (_recipient)
         _topic         = _recipient->name();
     else if (_entry)
         _topic         = _entry->title().isEmpty() ? _entry->text() : _entry->title();
     else
         _topic.clear();

     if (!_messages)
     {
         auto last = new Message(data.value("last_message").toObject(), this);
         _messages          = new MessagesModel(last, this);
     }

     auto users = data.value("users").toArray();
     foreach(auto userData, users)
        _users << new User(userData.toObject(), this); // TODO: isOnline
             
     users = data.value("users_deleted").toArray();
     foreach(auto userData, users)
        _deletedUsers << new User(userData.toObject(), this);

     users = data.value("users_left").toArray();
     foreach(auto userData, users)
        _leftUsers << new User(userData.toObject(), this);

     Tasty::instance()->pusher()->addChat(this);

     emit updated();
}



void Conversation::_setNotLoading()
{
    _loading = false;
    emit loadingChanged();
}



void Conversation::_emitTyped(int userId)
{
    emit typed(author(userId));
}



int Conversation::totalCount() const
{
    return _totalCount;
}



Author* Conversation::author(int id)
{
    if (id <= 0)
        return nullptr;

    if (_allUsers.contains(id))
        return _allUsers.value(id);

    auto author = new Author(this);
    author->setId(id);
    _allUsers[id] = author;
    return author;
}



void Conversation::sendMessage(const QString text)
{
    if (_loading || _id <= 0)
        return;

    _loading = true;
//    emit loadingChanged();

    auto content = QUrl::toPercentEncoding(text.trimmed());
    auto uuid    = QUuid::createUuid().toString();

    qDebug() << uuid;

    auto data    = QString("content=%1&uuid=%2").arg(QString::fromUtf8(content)).arg(uuid).remove('{').remove('}');
    auto url     = QString("v2/messenger/conversations/by_id/%1/messages.json").arg(_id);
    auto request = new ApiRequest(url, true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(messageSent(const QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),        this, SLOT(_setNotLoading())));
}



void Conversation::readAll()
{
    if (_unreadCount <= 0 || _id <= 0)
        return;

    auto url = QString("v2/messenger/conversations/by_id/%1/messages/read_all.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation);

    Q_UNUSED(request);
}
