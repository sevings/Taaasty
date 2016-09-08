#include "MessageBase.h"

#include "../tasty.h"



MessageBase::MessageBase(QObject* parent)
    : TastyData(parent)
    , _user(nullptr)
    , _read(true)
    , _containsImage(false)
{

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
    _containsImage = _text.contains("<img", Qt::CaseInsensitive);
}



void MessageBase::_setDate(const QString d)
{
    _date = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
}



User* MessageBase::user() const
{
    return _user;
}



QString MessageBase::text() const
{
    return _text;
}
