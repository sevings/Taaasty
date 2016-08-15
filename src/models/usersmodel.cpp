#include "usersmodel.h"

#include <QDebug>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include "../defines.h"



UsersModel::UsersModel(QObject* parent)
    : QAbstractListModel(parent)
    , _loading(false)
{
    setMode(FollowingsMode);
}



QHash<int, QByteArray> UsersModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "user";
    return roles;
}

bool UsersModel::loading() const
{
    return _loading;
}

