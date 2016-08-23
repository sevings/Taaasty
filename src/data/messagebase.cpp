#include "messagebase.h"

#include "../tasty.h"



MessageBase::MessageBase(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _user(nullptr)
    , _read(true)
{

}



int MessageBase::id() const
{
    return _id;
}



bool MessageBase::isRead() const
{
    return _read;
}



QDateTime MessageBase::createdDate() const
{
    return _date;
}



void MessageBase::_setTruncatedText()
{
    _truncatedText = Tasty::truncateHtml(_text);
}



void MessageBase::_setDate(const QString d)
{
    _date = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
}
