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
#include "Comment.h"

#include "../models/messagesmodel.h"
#include "../models/commentsmodel.h"
#include "../models/chatsmodel.h"



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



Conversation::Conversation(Entry* entry)
    : QObject(entry)
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
    , _messages(new MessagesModel(this))
    , _loading(false)
{
    setEntryId(entry->entryId());

    _entry = entry;
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
     _lastMessage       = new Message(data.value("last_message").toObject(), this, this);

     if (!_messages)
     {
         _messages      = new MessagesModel(this);

         Q_TEST(connect(_messages, SIGNAL(lastMessageChanged()), this, SIGNAL(lastMessageChanged())));
     }
     else
         _messages->reset();

     if (!_entry && data.contains("entry"))
     {
        _entry          = new Entry(data.value("entry").toObject(), this);

        Q_TEST(connect(_entry->commentsModel(), SIGNAL(lastCommentChanged()), this, SIGNAL(lastMessageChanged())));
     }

     delete _recipient;
     if (data.contains("recipient"))
         _recipient     = new Author(data.value("recipient").toObject(), this);
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

     auto users = data.value("users").toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this); // TODO: isOnline
        _users.insert(user->id(), user);
     }
             
     users = data.value("users_deleted").toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this);
        _deletedUsers.insert(user->id(), user);
     }

     users = data.value("users_left").toArray();
     foreach(auto userData, users)
     {
        auto user = new User(userData.toObject(), this);
        _leftUsers.insert(user->id(), user);
     }

     Tasty::instance()->pusher()->addChat(this);

     emit lastMessageChanged();
     emit updated();
     emit unreadCountChanged();
}



void Conversation::_setNotLoading()
{
    _loading = false;
    emit loadingChanged();
}



void Conversation::_markRead(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
    {
        qDebug() << "error read chat" << _id;
        return;
    }

    _unreadCount = _unreceivedCount;
    emit unreadCountChanged();
}



MessagesModel* Conversation::messages() const
{
    return _messages;
}



User* Conversation::user(int id)
{
    if (_users.contains(id))
        return _users.value(id);

    if (!_isAnonymous)
    {
        auto user = new User(this);
        user->setId(id);
        _users[id] = user;
        return user;
    }

    if (!_users.contains(0))
        _users[0] = new User(this);

    return _users[0];
}



MessageBase* Conversation::lastMessage() const
{
    MessageBase* last = nullptr;

    if (_entry)
        last = _entry->commentsModel()->lastComment();

    if (last)
        return last;

    last = _messages->lastMessage();
    if (last)
        return last;

    return _lastMessage;
}



bool Conversation::isAnonymous() const
{
    return _isAnonymous;
}



int Conversation::totalCount() const
{
    return _totalCount;
}



void Conversation::sendMessage(const QString text)
{
    if (_loading || _id <= 0)
        return;

    _loading = true;
//    emit loadingChanged();

    auto content = QUrl::toPercentEncoding(text.trimmed());
    auto uuid    = QUuid::createUuid().toString().remove('{').remove('}');
    auto data    = QString("uuid=%1&content=%2").arg(uuid).arg(QString::fromUtf8(content));
    auto url     = QString("v2/messenger/conversations/by_id/%1/messages.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PostOperation, data);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(messageSent(const QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),        this, SLOT(_setNotLoading())));

    if (_unreadCount > 0)
        Q_TEST(connect(request, SIGNAL(success(QJsonObject)),   this, SLOT(readAll())));
}



void Conversation::readAll()
{
    if (_unreadCount <= 0 || _id <= 0)
        return;

    auto url = QString("v2/messenger/conversations/by_id/%1/messages/read_all.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(allMessagesRead(const QJsonObject))));
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)),       this, SLOT(_markRead(QJsonObject))));
}
