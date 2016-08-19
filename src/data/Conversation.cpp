#include "Conversation.h"

#include <QRegularExpression>
#include <QUuid>
#include <QDebug>

#include "../defines.h"

#include "../apirequest.h"
#include "../pusherclient.h"
#include "../tasty.h"
#include "../settings.h"

#include "Entry.h"
#include "Message.h"
#include "User.h"
#include "Author.h"
#include "Tlog.h"
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
    , _entryId(0)
    , _recipient(nullptr)
    , _messages(nullptr)
    , _loading(false)
    , _reading(false)
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
    , _entryId(0)
    , _recipient(nullptr)
    , _messages(new MessagesModel(this))
    , _loading(false)
    , _reading(false)
{
    setEntryId(entry->entryId());
}



Conversation::Conversation(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entryId(0)
    , _recipient(nullptr)
    , _messages(nullptr)
    , _loading(false)
    , _reading(false)
{
    _init(data);
}



Conversation::~Conversation()
{
    Tasty::instance()->pusher()->removeChat(this);
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

    Tasty::instance()->pusher()->addChat(this);

    update();
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
    if (_loading || entryId <= 0 || _entryId == entryId)
        return;

    _entryId = entryId;

    Tasty::instance()->pusher()->addChat(this);

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
     _canTalk           = data.value("can_talk").toBool(true);
     _canDelete         = data.value("can_delete").toBool(true);
     _isAnonymous       = data.value("is_anonymous").toBool();

     Tasty::instance()->pusher()->addChat(this);

     if (!_messages)
     {
         _messages      = new MessagesModel(this);

         Q_TEST(connect(_messages, SIGNAL(lastMessageChanged()), this, SIGNAL(lastMessageChanged())));
     }

     if (data.contains("entry"))
     {
         _entryData = data.value("entry").toObject();
         _entryId = _entryData.value("id").toInt();
         auto entry = Tasty::instance()->pusher()->entry(_entryId);
         if (entry)
             entry->setParent(this);
         else
             entry = new Entry(_entryData, this);

         Tasty::instance()->pusher()->addChat(this);

         Q_TEST(connect(entry->commentsModel(), SIGNAL(lastCommentChanged()), this, SIGNAL(lastMessageChanged())));
     }

     if (!_recipient && data.contains("recipient"))
         _recipient     = new Author(data.value("recipient").toObject(), this);

     if (data.contains("topic"))
         _topic         = data.value("topic").toString();
     else if (_recipient)
         _topic         = _recipient->name();
     else if (_entryId)
     {
         auto e = entry();
         _topic         = e->title().isEmpty() ? e->text() : e->title();
         _topic.remove(QRegularExpression("<[^>]*>"))
                 .replace('\n', ' ').truncate(100);
     }
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

     _lastMessage       = new Message(data.value("last_message").toObject(), this, this);

     emit isInvolvedChanged();
     emit unreadCountChanged();
     emit lastMessageChanged();
     emit updated();
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

    _reading = false;
    _unreadCount = 0;
    emit unreadCountChanged();
}



Conversation::ConversationType Conversation::type() const
{
    return _type;
}



bool Conversation::isInvolved() const
{
    if (_type == PrivateConversation)
        return true;

    if (_userId <= 0)
        return false;

    return _users.contains(_userId)
            && !_leftUsers.contains(_userId)
            && !_deletedUsers.contains(_userId);
}



bool Conversation::isMyLastMessageUnread() const
{
    return _lastMessage->userId() == _userId && !_lastMessage->isRead();
}



MessagesModel* Conversation::messages() const
{
    return _messages;
}



User* Conversation::user(int id)
{
    if (_users.contains(id))
        return _users.value(id);

    if (id == _recipientId && _recipient)
        return _recipient;

    if (id == Tasty::instance()->settings()->userId())
    {
        auto user = new User(this);
        *user = *Tasty::instance()->me();
        _users.insert(id, user);
        return user;
    }

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



MessageBase* Conversation::lastMessage()
{
    MessageBase* last = _lastMessage;

    if (_entryId)
    {
        auto lst = entry()->commentsModel()->lastComment();
        if (lst && lst->createdDate() > last->createdDate())
            last = lst;
    }

    auto lst = _messages->lastMessage();
    if (lst && lst->createdDate() > last->createdDate())
        last = lst;

    return last;
}



bool Conversation::isAnonymous() const
{
    return _isAnonymous;
}



int Conversation::totalCount() const
{
    return _totalCount;
}



void Conversation::update()
{
    if (_id <= 0 || _loading)
        return;

    auto request = new ApiRequest(QString("/v2/messenger/conversations/by_id/%1.json").arg(_id), true);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject))));
    Q_TEST(connect(request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading())));

    _loading = true;
    emit loadingChanged();
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
    if (_reading || _unreadCount <= 0 || _id <= 0)// || !isInvolved())
        return;

    _reading = true;

    auto url = QString("v2/messenger/conversations/by_id/%1/messages/read_all.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SIGNAL(allMessagesRead(const QJsonObject))));
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)),       this, SLOT(_markRead(QJsonObject)))); //! \todo _setNotReading()
}



void Conversation::leave()
{
    if (_loading || !isInvolved())
        return;
    
    _loading = true;
    
    auto url = QString("v2/messenger/conversations/by_id/%1/leave.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation);

    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_emitLeft(QJsonObject))));
}



void Conversation::remove()
{
    if (_loading || !isInvolved())
        return;

    _loading = true;

    auto url = QString("v2/messenger/conversations/by_id/%1.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);

    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_emitLeft(QJsonObject))));
}



void Conversation::_emitLeft(const QJsonObject data)
{
    _loading = false;
    
    if (data.value("status").toString() != "success")
    {
        qDebug() << "error leave chat" << _id;
        return;
    }
    
    emit left(_id);

    auto user = _users.value(_userId);
    if (!user)
    {
        user = new User(this);
        *user = *Tasty::instance()->me();
    }

    _leftUsers.insert(user->id(), user);
    emit isInvolvedChanged();

    emit Tasty::instance()->info("Беседа удалена");
}



Entry* Conversation::entry()
{
    if (!_entryId)
        return nullptr;

    auto e = Tasty::instance()->pusher()->entry(_entryId);
    if (e)
        return e;

    e = new Entry(_entryData, this);
    _entryData = QJsonObject();
    e->setId(_entryId);
    return e;
}



int Conversation::unreadCount() const
{
    return _unreadCount;
}
