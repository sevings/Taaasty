#include "tastylistmodel.h"

#include "../defines.h"
#include "../apirequest.h"



TastyListModel::TastyListModel(QObject* parent)
    : QAbstractListModel(parent)
    , _hasMore(true)
{

}



bool TastyListModel::isLoading() const
{
    return _loadRequest;
}



bool TastyListModel::isChecking() const
{
    return _checkRequest;
}



bool TastyListModel::hasMore() const
{
    return _hasMore;
}



void TastyListModel::_initLoad(bool emitting)
{
    if (emitting)
        emit loadingChanged();

    if (!_loadRequest)
        return;

    Q_TEST(connect(_loadRequest, &QObject::destroyed,
            this, &TastyListModel::loadingChanged, Qt::QueuedConnection));
}



void TastyListModel::_initCheck(bool emitting)
{
    if (emitting)
        emit checkingChanged();

    if (!_checkRequest)
        return;

    Q_TEST(connect(_checkRequest, &QObject::destroyed,
            this, &TastyListModel::checkingChanged, Qt::QueuedConnection));
}
