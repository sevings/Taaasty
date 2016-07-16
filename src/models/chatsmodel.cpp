#include "chatsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../data/Conversation.h"



ChatsModel* ChatsModel::instance(QObject* parent)
{
    static auto model = new ChatsModel(parent);
    return model;
}



ChatsModel::ChatsModel(QObject* parent)
    : QAbstractListModel(parent)
    , _hasMore(true)
    , _url("v2/messenger/conversations.json?limit=10&page=%1")
    , _loading(false)
    , _page(1)
    , _request(nullptr)
{
    qDebug() << "ChatsModel";

    Q_TEST(connect(Tasty::instance(), SIGNAL(authorized()), this, SLOT(reset())));
}



int ChatsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return _chats.size();
}



QVariant ChatsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _chats.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Conversation*>(_chats.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool ChatsModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _hasMore;
}



void ChatsModel::fetchMore(const QModelIndex& parent)
{
    if (_loading || parent.isValid())
        return;

    qDebug() << "ChatsModel::fetchMore";

    _loading = true;

    QString url = _url.arg(_page++);
    _request = new ApiRequest(url, true);

    Q_TEST(connect(_request, SIGNAL(success(QJsonArray)), this, SLOT(_addChats(QJsonArray))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)), this, SLOT(_setNotLoading(QObject*))));
}



void ChatsModel::reset()
{
    beginResetModel();
    
    qDeleteAll(_chats);
    _chats.clear();

    _hasMore = true;
    _loading = false;
    _page = 1;

    delete _request;
    _request = nullptr;
    
    endResetModel();
}



QHash<int, QByteArray> ChatsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "chat";
    return roles;
}



void ChatsModel::_addChats(QJsonArray data)
{
    qDebug() << "ChatsModel::_addChats";

    if (data.isEmpty())
    {
        _hasMore = false;
        emit hasMoreChanged();
        
        _loading = false;
        _request = nullptr;
        return;
    }

    beginInsertRows(QModelIndex(), _chats.size(), _chats.size() + data.size() - 1);
    
    _chats.reserve(_chats.size() + data.size());
    foreach(auto item, data)
    {
        auto chat = new Conversation(item.toObject(), this);
        _chats << chat;
    }

    endInsertRows();
    
    _loading = false;
    _request = nullptr;

    if (_chats.size() < 10)
        emit hasMoreChanged();
}



void ChatsModel::_setNotLoading(QObject* request)
{
    if (request == _request)
    {
        _loading = false;
        _request = nullptr;
    }
}
