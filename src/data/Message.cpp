#include "Message.h"

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "../pusherclient.h"
//#include "Author.h"



Message::Message(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _userId(0)
    , _recipientId(0)
    , _conversationId(0)
    , _read(false)
//    , _author(new Author(this))
{
}



Message::Message(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Message::~Message()
{
    Tasty::instance()->pusher()->removeMessage(_id);
}



int Message::id() const
{
    return _id;
}



void Message::read()
{
    if (_id <= 0 || _read)
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
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
    _text           = data.value("content_html").toString(); // TODO: SystemMessage
//    _author         = new Author(data.value("author").toObject(), this);
    // _attachments    = data.value("attachments").toArray();
    
    _correctHtml();

    Tasty::instance()->pusher()->addMessage(this);

    emit readChanged();
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

    auto userId = data.value("read_user_id").toInt();
    if (userId <= 0)
        return;

    if (userId == _userId || userId == _recipientId)
    {
        _read = true;
        emit readChanged();
    }
}
