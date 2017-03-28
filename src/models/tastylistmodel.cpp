// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "tastylistmodel.h"



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



void TastyListModel::_initLoad()
{
    if (!_loadRequest)
        return;

    emit loadingChanged();

    _errorString.clear();
    emit errorStringChanged();

    _networkError = false;
    emit networkErrorChanged();

    Q_TEST(connect(_loadRequest, &QObject::destroyed,
            this, &TastyListModel::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_loadRequest, SIGNAL(error(int,QString)),
                   this, SLOT(_setErrorString(int,QString))));

    QPointer<TastyListModel> that(this);
    Q_TEST(connect(_loadRequest, &ApiRequest::networkError, [that]()
    {
        if (!that)
            return;

        that->_errorString = "Сетевая ошибка";
        emit that->errorStringChanged();
        
        that->_networkError = true;
        emit that->networkErrorChanged();
    }));
    
    _loadRequest->get();
}



void TastyListModel::_initCheck()
{
    if (!_checkRequest)
        return;

    emit checkingChanged();

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
