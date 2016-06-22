#include "usersmodelbayes.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "defines.h"

#include "apirequest.h"
#include "datastructures.h"
#include "bayes.h"



UsersModelBayes::UsersModelBayes(QObject* parent)
    : UsersModel(parent)
    , _tlogModel(nullptr)
{
    _mode = FireMode;

    _loadDb();
}



UsersModelBayes::~UsersModelBayes()
{
    _saveDb();
}



int UsersModelBayes::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _tlogs[_mode].size();
}



QVariant UsersModelBayes::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0)
        return QVariant();

    if (role != Qt::UserRole)
        return QVariant();

    if (index.row() >= _tlogs[_mode].size())
        return QVariant();

    auto tlog = _tlogs[_mode].at(index.row());
    if (tlog.user->id() <= 0)
        tlog.loadInfo();

    return QVariant::fromValue<User*>(tlog.user);

    qDebug() << "role" << role;

    return QVariant();
}



void UsersModelBayes::setMode(const UsersModel::Mode mode)
{
    _mode = mode;

    switch(mode)
    {
    case WaterMode:
    case FireMode:
        if (_tlogs[mode].isEmpty())
            _loadBayesTlogs(); //! TODO: but what if there are no users?
        break;
    default:
        qDebug() << "users mode =" << mode;
    }
}



void UsersModelBayes::_setBayesItems()
{
    if (!_tlogModel)
        return;

    beginInsertRows(QModelIndex(), 0, _tlogModel->_users.size() - 1);

    _tlogs[_mode].clear();
    foreach (auto user, _tlogModel->_users)
        _tlogs[_mode] << BayesTlog(user);

    endInsertRows();

//    delete _bayesModel;
    _tlogModel = nullptr;

    _loading = false;
    emit hasMoreChanged();
}



void UsersModelBayes::_initDb()
{
    QSqlQuery query;
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes_tlogs   (type INTEGER, tlog INTEGER, latest INTEGER, n INTEGER, PRIMARY KEY(tlog))"));
}



void UsersModelBayes::_loadDb()
{
    _initDb();

    QSqlQuery query;
    Q_TEST(query.exec("SELECT type, tlog, latest, n FROM bayes_tlogs WHERE tlog > 0 ORDER BY n"));
    while (query.next())
        _tlogs[query.value(0).toInt()] << BayesTlog(query.value(1).toInt(),
                                                    query.value(2).toInt());

//    Q_TEST(query.exec("SELECT latest FROM bayes_tlogs WHERE tlog = -1"));
//    if (query.next())
//        _lastFavorite = query.value(0).toInt();
}



void UsersModelBayes::_saveDb()
{
    QSqlQuery query;
    for (int type = 0; type < 2; type++)
        for (int tlog = 0; tlog < _tlogs[type].size(); tlog++)
            if (_tlogs[type].at(tlog).include && !_tlogs[type].at(tlog).removed)
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes_tlogs VALUES (?, ?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(_tlogs[type].at(tlog).id);
                query.addBindValue(_tlogs[type].at(tlog).latest);
                query.addBindValue(tlog);
                Q_TEST(query.exec());
            }
}



void UsersModelBayes::_loadBayesTlogs()
{
    _loading = true;
    emit hasMoreChanged();

//    delete _bayesModel;
    _tlogModel = new UsersModelTlog(this);

    Q_TEST(connect(_tlogModel, SIGNAL(downloadCompleted()), this, SLOT(_setBayesItems())));

    switch (_mode)
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



UsersModelBayes::BayesTlog UsersModelBayes::_findTlog(int id, bool included)
{
    for (int type = 0; type < 2; type++)
        foreach (auto tlog, _tlogs[type])
            if (tlog.id == id)
            {
                if (!tlog.removed
                        && ((included && tlog.include) || !included))
                    return tlog;
                else
                    return BayesTlog();
            }

    return BayesTlog();
}



UsersModelBayes::BayesTlog::BayesTlog(int userId, int last)
    : user(new User)
    , id(userId)
    , latest(last)
    , include(true)
    , removed(false)
{

}



UsersModelBayes::BayesTlog::BayesTlog(User* user)
    : user(user)
    , id(user->id())
    , latest(0)
    , include(true)
    , removed(false)
{

}



UsersModelBayes::BayesTlog::~BayesTlog()
{
//    if (!user->parent()) //TODO: delete user
//        delete user;
}



void UsersModelBayes::BayesTlog::loadInfo()
{
    user->setId(id);
}
