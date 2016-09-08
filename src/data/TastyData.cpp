#include "TastyData.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"



TastyData::TastyData(QObject* parent)
    : QObject(parent)
    , _id(0)
{

}



int TastyData::id() const
{
    return _id;
}



bool TastyData::isLoading() const
{
    return _request;
}



void TastyData::_initRequest(bool emitting)
{
    if (emitting)
        emit loadingChanged();
    
    if (!_request)
        return;
    
    Q_TEST(connect(_request, &QObject::destroyed, 
            this, &TastyData::loadingChanged, Qt::QueuedConnection));
}
