#include "messagebase.h"

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
