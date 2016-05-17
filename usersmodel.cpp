#include "usersmodel.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "defines.h"

#include "apirequest.h"
#include "datastructures.h"
#include "bayes.h"



UsersModel::UsersModel(QObject* parent)
    : QAbstractListModel(parent)
    , _lastFavorite(0)
    , _loadAll(false)
    , _tlog(0)
    , _total(1)
    , _loading(false)
    , _lastPosition(0)
{
    setMode(FollowingsMode);

    _loadDb();
}



UsersModel::~UsersModel()
{
    _saveDb();
}



int UsersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return ((_mode == WaterMode || _mode == FireMode) ? _tlogs[_mode].size() : _users.size());
}



QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0)
        return QVariant();

    if (role != Qt::UserRole)
        return QVariant();

    if (_mode == WaterMode || _mode == FireMode)
    {
        if (index.row() >= _tlogs[_mode].size())
            return QVariant();

        auto tlog = _tlogs[_mode].at(index.row());
        if (tlog.user->id() <= 0)
            tlog.loadInfo();

        return QVariant::fromValue<User*>(tlog.user);
    }

    if (index.row() >= _users.size())
        return QVariant();

    return QVariant::fromValue<User*>(_users.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool UsersModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid() || _mode == WaterMode || _mode == FireMode)
        return false;

    return _users.size() < _total;
}



void UsersModel::fetchMore(const QModelIndex& parent)
{
    if (_loading || !canFetchMore(QModelIndex()) || parent.isValid()
            || ((_mode == FollowingsMode || _mode == FollowersMode) && !_tlog))
        return;

    _loading = true;

    QString url = _url;
    if (_lastPosition)
        url += QString("&since_position=%1").arg(_lastPosition);

    auto request = new ApiRequest(url);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject)));
}



void UsersModel::setMode(const UsersModel::Mode mode)
{
    switch(mode)
    {
    case WaterMode:
    case FireMode:
        _url.clear();
        _field.clear();
        if (_tlogs[mode].isEmpty())
            _loadBayesTlogs(); //! TODO: but what if there are no users?
        break;
    case FollowersMode:
        _url = QString("tlog/%1/followers.json?limit=50").arg(_tlog);
        _field = "reader";
        break;
    case FollowingsMode:
        _url = QString("tlog/%1/followings.json?limit=50").arg(_tlog);
        _field = "user";
        break;
    case MyFollowingsMode:
        _url = QString("relationships/to/friend.json?limit=50");
        _field = "user";
        break;
    case MyFollowersMode:
        _url = QString("relationships/by/friend.json?limit=50"); //! TODO: test me
        _field = "reader";
        break;
    case MyIgnoredMode:
        _url = QString("relationships/to/ignored.json?limit=50");
        _field = "user";
        break;
    default:
        qDebug() << "users mode =" << mode;
    }

    _mode = mode;
}



void UsersModel::setTlog(const int tlog)
{
    if (tlog <= 0)
        return;

    beginResetModel();

    _tlog = tlog;
    _total = 1;
    _loading = false;
    _lastPosition = 0;
    
    setMode(_mode);
    
    qDeleteAll(_users);

    endResetModel();

    emit hasMoreChanged();
}



QHash<int, QByteArray> UsersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "user";
    return roles;
}



void UsersModel::_addItems(QJsonObject data)
{
    _total = data.value("total_count").toInt();

    auto list = data.value("relationships").toArray();
    if (list.isEmpty())
    {
        _total = _users.size();
        emit hasMoreChanged();
        _loading = false;

        if (_loadAll)
            _saveBayesTlogs();

        return;
    }

    _lastPosition = list.last().toObject().value("position").toInt();
    
    if (_loadAll)
        beginInsertRows(QModelIndex(), _tlogs[_mode].size(), _tlogs[_mode].size() + list.size() - 1);
    else
        beginInsertRows(QModelIndex(), _users.size(), _users.size() + list.size() - 1);

    QList<User*> users;
    users.reserve(list.size());
    foreach(auto item, list)
    {
        auto data = item.toObject().value(_field).toObject();
        auto user = new User(data, this);
        users << user;
    }

    if (_loadAll)
        _saveBayesTlogs(users);
    else
        _users << users;

    endInsertRows();

    _loading = false;
}



void UsersModel::_initDb()
{
    QSqlQuery query;
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes_tlogs   (type INTEGER, tlog INTEGER, latest INTEGER, n INTEGER, PRIMARY KEY(tlog))"));
}



void UsersModel::_loadDb()
{
    _initDb();

    QSqlQuery query;
    Q_TEST(query.exec("SELECT type, tlog, latest, n FROM bayes_tlogs WHERE tlog > 0 ORDER BY n"));
    while (query.next())
        _tlogs[query.value(0).toInt()] << BayesTlog(query.value(1).toInt(),
                                                    query.value(2).toInt());

    Q_TEST(query.exec("SELECT latest FROM bayes_tlogs WHERE tlog = -1"));
    if (query.next())
        _lastFavorite = query.value(0).toInt();
}



void UsersModel::_saveDb()
{
    QSqlQuery query;
    for (int type = 0; type < 2; type++)
        for (int tlog = 0; tlog < _tlogs[type].size(); tlog++)
            if (_tlogs[type].at(tlog).include && !_tlogs[type].at(tlog).removed)
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes_tlogs VALUES (?, ?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(_tlogs[tlog].at(tlog).id);
                query.addBindValue(_tlogs[tlog].at(tlog).latest);
                query.addBindValue(tlog);
                Q_TEST(query.exec());
            }
}



void UsersModel::_loadBayesTlogs()
{
    _loadAll = true;

    if (_tlogs[WaterMode].isEmpty())
    {
        setMode(MyIgnoredMode);
        fetchMore(QModelIndex());
    }
    else if (_tlogs[FireMode].isEmpty())
    {
        setMode(MyFollowingsMode);
        fetchMore(QModelIndex());
    }
    else
        _loadAll = false;
}



void UsersModel::_saveBayesTlogs(QList<User*> users)
{
    if (!_loadAll)
        return;

    Mode mode;
    switch (_mode)
    {
    case MyIgnoredMode:
        mode = WaterMode;
        break;
    case MyFollowingsMode:
        mode = FireMode;
        break;
    default:
        return;
    }

    _tlogs[mode].reserve(_tlogs[mode].size() + users.size());
    foreach(auto user, users)
        _tlogs[mode] << BayesTlog(user);

    if (canFetchMore(QModelIndex()))
    {
        fetchMore(QModelIndex());
        return;
    }

    if (mode == WaterMode)
    {
        setMode(MyFollowingsMode);
        fetchMore(QModelIndex());
    }
    else
    {
        _saveDb();
        _loadAll = false;
    }
}



UsersModel::BayesTlog UsersModel::_findTlog(int id, bool included)
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



UsersModel::BayesTlog::BayesTlog(int userId, int last)
    : user(new User)
    , id(userId)
    , latest(last)
    , include(true)
    , removed(false)
{

}



UsersModel::BayesTlog::BayesTlog(User* user)
    : user(user)
    , id(user->id())
    , latest(0)
    , include(true)
    , removed(false)
{

}



UsersModel::BayesTlog::~BayesTlog()
{
//    delete user;
}



void UsersModel::BayesTlog::loadInfo()
{
//    tlog->setId(id);
}
