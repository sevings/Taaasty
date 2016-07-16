#include "Message.h"

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "Author.h"



Message::Message(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _userId(0)
    , _recipientId(0)
    , _conversationId(0)
    , _author(new Author(this))
{

}



Message::Message(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);
}



int Message::id() const
{
    return _id;
}



void Message::_init(const QJsonObject data)
{
    _id             = data.value("id").toInt();
    _userId         = data.value("user_id").toInt();
    _recipientId    = data.value("recipient_id").toInt();
    _conversationId = data.value("conversation_id").toInt();
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
    _content        = data.value("content_html").toString();
    _author         = new Author(data.value("").toObject(), this);
    // _attachments    = data.value("attachments").toArray();
    
    emit updated();
}
