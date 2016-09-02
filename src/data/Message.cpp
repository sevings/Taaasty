#include "Message.h"

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "../settings.h"
#include "../pusherclient.h"
#include "../models/chatsmodel.h"
#include "../models/attachedimagesmodel.h"

#include "User.h"
#include "Conversation.h"



Message::Message(QObject* parent)
    : MessageBase(parent)
    , _userId(0)
    , _recipientId(0)
    , _conversationId(0)
    , _replyTo(nullptr)
    , _replyUserId(0)
    , _chat(nullptr)
    , _attachedImagesModel(nullptr)
{
    _user = new User(this);
}



Message::Message(const QJsonObject data, Conversation* chat, QObject *parent)
    : MessageBase(parent)
    , _replyTo(nullptr)
    , _chat(chat)
    , _attachedImagesModel(nullptr)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));

    if (_userId != Tasty::instance()->settings()->userId())
        Q_TEST(connect(chat, SIGNAL(allMessagesRead(QJsonObject)), this, SLOT(_markRead(QJsonObject))));
}



Message::~Message()
{
    Tasty::instance()->pusher()->removeMessage(_id);
}



int Message::id() const
{
    return _id;
}



int Message::userId() const
{
    return _userId;
}



User* Message::replyTo()
{
    if (_replyUserId <= 0)
        return nullptr;

    if (!_replyTo)
        _replyTo = _chat->user(_replyUserId);

    return _replyTo;
}



void Message::read()
{
    if (_read || _id <= 0 || _userId == Tasty::instance()->settings()->userId())
        return;

    auto url = QString("v2/messenger/conversations/by_id/%1/messages/read.json").arg(_conversationId);
    auto data = QString("ids=%1").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation, data);

    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_markRead(QJsonObject))));
}



void Message::_init(const QJsonObject data)
{
    _id             = data.value("id").toInt();
    _userId         = data.value("user_id").toInt();
    _recipientId    = data.value("recipient_id").toInt();
    _conversationId = data.value("conversation_id").toInt();
    _read           = !data.value("read_at").isNull();
    auto d = data.value("created_at").toString();
    _createdAt      = Tasty::parseDate(d, _chat && _chat->type() == Conversation::PrivateConversation);
    _setDate(d);
    _text           = data.value("content_html").toString().replace("&amp;", "&"); // TODO: SystemMessage

    _user = _chat->user(_userId);
    Q_ASSERT(_user);
    
    auto imageAttach = data.value("attachments").toArray();
    delete _attachedImagesModel;
    if (imageAttach.isEmpty())
        _attachedImagesModel = nullptr;
    else
        _attachedImagesModel = new AttachedImagesModel(&imageAttach, this);
        
    _correctHtml();
    _setTruncatedText();

    auto reply = data.value("reply_message").toObject();
    _replyUserId = reply.value("user_id").toInt();

    Tasty::instance()->pusher()->addMessage(this);

    emit readChanged();
    emit baseUpdated();
    emit updated();
}



void Message::_correctHtml()
{
    Tasty::correctHtml(_text, false);

    emit textUpdated();
}



void Message::_markRead(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
    {
        qDebug() << "error read message" << _id;
        return;
    }

    _read = true;
    emit readChanged();
}



void Message::_updateRead(const QJsonObject data)
{
    if (_read || data.value("id").toInt() != _id)
        return;

    _read = !data.value("read_at").isNull();
    emit readChanged();
}



void Message::_markRemoved(const QJsonObject data)
{
    if (data.value("id").toInt() != _id)
        return;

    _text = data.value("content").toString();
//    _type = data.value("type").toString();
    _setTruncatedText();

    emit textUpdated();
}
