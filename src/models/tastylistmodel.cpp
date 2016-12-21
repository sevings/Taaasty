// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "tastylistmodel.h"

#include "../defines.h"
#include "../apirequest.h"



TastyListModel::TastyListModel(QObject* parent)
    : QAbstractListModel(parent)
    , _hasMore(true)
    , _networkError(false)
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



bool TastyListModel::networkError() const
{
    return _networkError;
}



QString TastyListModel::errorString() const
{
    return _errorString;
}



void TastyListModel::loadMore()
{
    fetchMore(QModelIndex());
}



void TastyListModel::_setErrorString(int errorCode, QString str)
{
    qDebug() << "TastyListModel error code" << errorCode;

    _errorString = str;
    emit errorStringChanged();

    if (_networkError)
    {
        _networkError = false;
        emit networkErrorChanged();
    }
}



void TastyListModel::_initLoad(bool emitting)
{
    if (emitting)
        emit loadingChanged();

    if (!_loadRequest)
        return;

    _errorString.clear();
    emit errorStringChanged();

    _networkError = false;
    emit networkErrorChanged();

    Q_TEST(connect(_loadRequest, &QObject::destroyed,
            this, &TastyListModel::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_loadRequest, SIGNAL(error(int,QString)),
                   this, SLOT(_setErrorString(int,QString))));

    Q_TEST(connect(_loadRequest, &ApiRequest::networkError, [this]()
    {
        _errorString = "Сетевая ошибка";
        emit errorStringChanged();
        
        _networkError = true;
        emit networkErrorChanged();
    }));
    
    _loadRequest->get();
}



void TastyListModel::_initCheck(bool emitting)
{
    if (emitting)
        emit checkingChanged();

    if (!_checkRequest)
        return;

    Q_TEST(connect(_checkRequest, &QObject::destroyed,
                   this, &TastyListModel::checkingChanged, Qt::QueuedConnection));
                   
    _checkRequest->get();
}



ApiRequest::Options TastyListModel::_optionsForFetchMore(bool accessTokenRequired) const
{
    ApiRequest::Options options = accessTokenRequired ? ApiRequest::AccessTokenRequired
                                                      : ApiRequest::NoOptions;
    if (rowCount())
        options |= ApiRequest::ShowMessageOnError;
    return options;
}
