#include "Message.h"

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
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
    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
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
    _read           = !data.value("read_at").isNull();
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
    _text           = data.value("content_html").toString(); // TODO: SystemMessage
//    _author         = new Author(data.value("author").toObject(), this);
    // _attachments    = data.value("attachments").toArray();
    
    _correctHtml();

    emit read();
    emit updated();
}

void Message::_correctHtml()
{
    Tasty::correctHtml(_text, false);

    emit textUpdated();
}
