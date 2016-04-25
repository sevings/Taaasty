#include "usersmodel.h"

#include <QDebug>

#include "apirequest.h"
#include "datastructures.h"



UsersModel::UsersModel(QObject* parent)
    : QAbstractListModel(parent)
    , _tlog(0)
    , _total(1)
    , _loading(false)
    , _lastPosition(0)
{
    setMode(FollowingsMode);
}



UsersModel::~UsersModel()
{
    // qDeleteAll(_users);
}



int UsersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _users.size();
}



QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _users.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<User*>(_users.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool UsersModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _users.size() < _total;
}



void UsersModel::fetchMore(const QModelIndex& parent)
{
//    qDebug() << "fetch more";

    if (_loading ||  _users.size() >= _total ||parent.isValid() || (_mode < MyFollowingsMode && !_tlog))
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
        qDebug() << "feed mode =" << mode;
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
//    qDebug() << "adding entries";

    _total = data.value("total_count").toInt();

    auto list = data.value("relationships").toArray();
    if (list.isEmpty())
    {
        _total = _users.size();
        emit hasMoreChanged();
        return;
    }

    _lastPosition = list.last().toObject().value("position").toInt();
    
    beginInsertRows(QModelIndex(), _users.size(), _users.size() + list.size() - 1);

    _users.reserve(_users.size() + list.size());
    foreach(auto item, list)
    {
        auto data = item.toObject().value(_field).toObject();
        auto user = new User(data, this);
        _users << user;
    }

    endInsertRows();

    _loading = false;
}
