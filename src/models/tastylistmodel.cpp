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



QString TastyListModel::errorString() const
{
    return _errorString;
}



void TastyListModel::_setErrorString(int errorCode, QString str)
{
    qDebug() << "TastyListModel error code" << errorCode;

    _errorString = str;
    emit errorStringChanged();
}



void TastyListModel::_initLoad(bool emitting)
{
    if (emitting)
        emit loadingChanged();

    if (!_loadRequest)
        return;

    _errorString.clear();
    emit errorStringChanged();

    Q_TEST(connect(_loadRequest, &QObject::destroyed,
            this, &TastyListModel::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_loadRequest, SIGNAL(error(int,QString)),
                   this, SLOT(_setErrorString(int,QString))));
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



ApiRequest::Options TastyListModel::_optionsForFetchMore(bool accessTokenRequired) const
{
    ApiRequest::Options options = accessTokenRequired ? ApiRequest::AccessTokenRequired
                                                      : ApiRequest::NoOptions;
    if (rowCount())
        options |= ApiRequest::ShowMessageOnError;
    return options;
}
