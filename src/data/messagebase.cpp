#include "messagebase.h"

#include <QRegularExpression>



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
    _truncatedText = _text;
    _truncatedText.remove(QRegularExpression("<[^>]*>"))
            .replace('\n', ' ').truncate(100);
}



void MessageBase::_setDate(const QString d)
{
    _date = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
}
