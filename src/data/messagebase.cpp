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



void MessageBase::_setTruncatedText()
{
    _truncatedText = _text;
    _truncatedText.remove(QRegularExpression("<[^>]*>"))
            .replace('\n', ' ').truncate(100);
}
