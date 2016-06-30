#include "usersmodelbayes.h"

#include <QDebug>

#include <QVariant>

#include "../defines.h"

#include "../apirequest.h"
#include "../data/User.h"
#include "../nbc/bayes.h"
#include "../nbc/trainer.h"



UsersModelBayes::UsersModelBayes(QObject* parent)
    : UsersModel(parent)
    , _trainer(Bayes::instance()->trainer())
    , _tlogModel(nullptr)
{
    _mode = FireMode;
}



UsersModelBayes::~UsersModelBayes()
{
    _trainer->_saveDb();
}



int UsersModelBayes::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _trainer->_tlogs[_mode].size();
}



QVariant UsersModelBayes::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0)
        return QVariant();

    if (role != Qt::UserRole)
        return QVariant();

    if (index.row() >= _trainer->_tlogs[_mode].size())
        return QVariant();

    auto tlog = _trainer->_tlogs[_mode].at(index.row());
    if (tlog.user->id() <= 0)
        tlog.loadInfo();

    return QVariant::fromValue<User*>(tlog.user);
}



void UsersModelBayes::setMode(const UsersModel::Mode mode)
{
    _mode = mode;

    switch(mode)
    {
    case WaterMode:
    case FireMode:
        if (_trainer->_tlogs[mode].isEmpty())
            _loadBayesTlogs(); //! TODO: but what if there are no users?
        break;
    default:
        qDebug() << "users mode =" << mode;
    }
}



void UsersModelBayes::removeUser(int id)
{
    int type = -1;
    auto i = _trainer->_findTlog(type, id);
    if (i < 0 || type < 0)
        return;

    if (type == _mode)
        beginRemoveRows(QModelIndex(), i, i);

    _trainer->_tlogs[type].removeAt(i);

    if (type == _mode)
        endRemoveRows();
}



void UsersModelBayes::_setBayesItems()
{
    if (!_tlogModel)
        return;

    beginInsertRows(QModelIndex(), 0, _tlogModel->_users.size() - 1);

    _trainer->_tlogs[_mode].clear();
    foreach (auto user, _tlogModel->_users)
        _trainer->_tlogs[_mode] << Trainer::BayesTlog(user);

    endInsertRows();

//    delete _bayesModel;
    _tlogModel = nullptr;

    _loading = false;
    emit hasMoreChanged();
}



void UsersModelBayes::_loadBayesTlogs()
{
    _loading = true;
    emit hasMoreChanged();

//    delete _bayesModel;
    _tlogModel = new UsersModelTlog(_trainer);

    Q_TEST(connect(_tlogModel, SIGNAL(downloadCompleted()), this, SLOT(_setBayesItems())));

    switch (_mode) // TODO: load both types
    {
    case WaterMode:
        _tlogModel->setMode(MyIgnoredMode);
        _tlogModel->downloadAll();
        break;
    case FireMode:
        _tlogModel->setMode(MyFollowingsMode);
        _tlogModel->downloadAll();
        break;
    default:
        return;
    }
}
