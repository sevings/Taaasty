#include "messagebase.h"

MessageBase::MessageBase(QObject *parent)
    : QObject(parent)
    , _id(0)
{

}



int MessageBase::id() const
{
    return _id;
}
