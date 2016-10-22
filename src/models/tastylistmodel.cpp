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



void TastyListModel::_setErrorString(int errorCode)
{
    switch (errorCode)
    {
    case 403:
        _errorString = "Доступ запрещен";
        break;
    case 404:
        _errorString = "Страница не найдена";
        break;
    default:
        qDebug() << "TastyListModel error code" << errorCode;
        _errorString = QString("При загрузке проиошла ошибка %1").arg(errorCode);
        break;
    }

    emit errorStringChanged();
}



void TastyListModel::_initLoad(bool emitting)
{
    if (emitting)
        emit loadingChanged();

    if (!_loadRequest)
        return;

    _errorString.clear();

    Q_TEST(connect(_loadRequest, &QObject::destroyed,
            this, &TastyListModel::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_loadRequest, SIGNAL(error(int,QString)),
                   this, SLOT(_setErrorString(int))));
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
