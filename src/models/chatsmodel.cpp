#include "chatsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>
#include <QQmlEngine>

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../pusherclient.h"
#include "../data/Author.h"
#include "../data/Entry.h"



ChatsModel* ChatsModel::instance(QObject* parent)
{
    static auto model = new ChatsModel(parent);
    return model;
}



ChatsModel::ChatsModel(QObject* parent)
    : QAbstractListModel(parent)
    , _mode(AllChatsMode)
    , _hasMore(true)
    , _url("v2/messenger/conversations.json?limit=10&page=%1")
    , _loading(false)
    , _checking(false)
    , _page(1)
    , _request(nullptr)
{
    qDebug() << "ChatsModel";

    Q_TEST(connect(Tasty::instance(),           SIGNAL(authorized()),       this, SLOT(reset())));
    Q_TEST(connect(Tasty::instance()->pusher(), SIGNAL(unreadChat()),       this, SLOT(loadUnread())));
    Q_TEST(connect(Tasty::instance()->pusher(), SIGNAL(unreadChats(int)),   this, SLOT(_checkUnread(int))));
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
        return QVariant::fromValue<Conversation*>(_chats.at(index.row()).data());

    qDebug() << "role" << role;

    return QVariant();
}



bool ChatsModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid() || !Tasty::instance()->isAuthorized())
        return false;

    return _hasMore;
}



void ChatsModel::fetchMore(const QModelIndex& parent)
{
    if (_loading || parent.isValid() || !Tasty::instance()->isAuthorized())
        return;

    qDebug() << "ChatsModel::fetchMore";

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_page);
    _request = new ApiRequest(url, true);

    Q_TEST(connect(_request, SIGNAL(success(QJsonArray)), this, SLOT(_addChats(QJsonArray))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)), this, SLOT(_setNotLoading(QObject*))));
}



void ChatsModel::setMode(ChatsModel::Mode mode)
{
    if (mode == _mode)
        return;
    
    beginResetModel();

    _mode = mode;
    emit modeChanged();
    
    if (_allChats.isEmpty())
        _allChats = _chats;

    _chats.clear();

    switch (_mode)
    {
    case AllChatsMode:
        _chats = _allChats;
        break;
    case PrivateChatsMode:
        foreach (auto chat, _allChats)
            if (chat->type() == Conversation::PrivateConversation
                    || chat->type() == Conversation::GroupConversation)
                _chats << chat;
        break;
    case EntryChatsMode:
        foreach (auto chat, _allChats)
            if (chat->type() == Conversation::PublicConversation)
                _chats << chat;
        break;
    default:
        qDebug() << "Error ChatsModel::Mode" << mode;
    }
    
    endResetModel();
}



void ChatsModel::addChat(EntryPtr entry)
{
    auto chat = entry->chat()->sharedFromThis();
    if (_ids.contains(chat->id()))
    {
        _bubbleChat(chat->id());
        return;
    }

    beginInsertRows(QModelIndex(), 0, 0);

    _allChats.prepend(chat);
    _chats.prepend(chat);
    _ids << chat->id();

    Q_TEST(connect(chat.data(), SIGNAL(left(int)), this, SLOT(_removeChat(int))));

    endInsertRows();
}



void ChatsModel::loadUnread()
{
    if (_checking)
        return;

    qDebug() << "ChatsModel::loadUnread";

    _checking = true;

    QString url("v2/messenger/conversations.json?unread=true");
    _request = new ApiRequest(url, true);

    Q_TEST(connect(_request, SIGNAL(success(QJsonArray)), this, SLOT(_addUnread(QJsonArray))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)), this, SLOT(_setNotChecking(QObject*))));
}



void ChatsModel::reset()
{
    beginResetModel();

    _allChats.clear();
    _chats.clear();        
    _ids.clear();

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



void ChatsModel::_addUnread(QJsonArray data)
{
    qDebug() << "ChatsModel::_addUnread";

    _request = nullptr;
    _checking = false;

    if (data.isEmpty())
        return;

    QList<int> bubbleIds;
    QList<ChatPtr> chats;
    foreach(auto item, data)
    {
        auto id = item.toObject().value("id").toInt();
        auto chat = Tasty::instance()->pusher()->chat(id);
        if (!chat)
        {
            chat = ChatPtr::create((QObject*)nullptr);
            chat->init(item.toObject());
        }

        if (_ids.contains(id))
        {
            if (_mode == AllChatsMode
                    || (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
                    || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation))
                bubbleIds << chat->id();

            continue;
        }

        _allChats << chat;
        _ids << chat->id();

        Q_TEST(connect(chat.data(), SIGNAL(left(int)), this, SLOT(_removeChat(int))));

        if (_mode == AllChatsMode
                || (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
                || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation))
            chats << chat;
    }

    foreach (auto id, bubbleIds)
        _bubbleChat(id);

    if (chats.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, chats.size() - 1);

    for (int i = 0; i < chats.size(); i++)
        _chats.insert(i, chats.at(i));

    endInsertRows();
}



void ChatsModel::_addChats(QJsonArray data)
{
    qDebug() << "ChatsModel::_addChats";

    _request = nullptr;
    _page++;

    if (data.isEmpty())
    {
        _hasMore = false;
        emit hasMoreChanged();

        _loading = false;
        emit loadingChanged();
        return;
    }

    QList<ChatPtr> chats;
    foreach(auto item, data)
    {
        auto id = item.toObject().value("id").toInt();
        if (_ids.contains(id))
            continue;

        auto chat = Tasty::instance()->pusher()->chat(id);
        if (!chat)
        {
            chat = ChatPtr::create((QObject*)nullptr);
            chat->init(item.toObject());
        }

        _ids << chat->id();
        _allChats << chat;

        Q_TEST(connect(chat.data(), SIGNAL(left(int)), this, SLOT(_removeChat(int))));

        if (_mode == AllChatsMode
                || (_mode == PrivateChatsMode && chat->type() != Conversation::PublicConversation)
                || (_mode == EntryChatsMode && chat->type() == Conversation::PublicConversation))
            chats << chat;
    }

    if (chats.isEmpty())
    {
        _loading = false;
        fetchMore(QModelIndex());
        return;
    }

    beginInsertRows(QModelIndex(), _chats.size(), _chats.size() + chats.size() - 1);
    _chats << chats;
    endInsertRows();
    
    if (_chats.size() < 10) //! \todo what is this?
        emit hasMoreChanged();

    _loading = false;
    emit loadingChanged();
}



void ChatsModel::_setNotLoading(QObject* request)
{
    if (request != _request)
        return;

    if (_loading)
    {
        _loading = false;
        emit loadingChanged();
    }

    _request = nullptr;
}



void ChatsModel::_setNotChecking(QObject* request)
{
    Q_UNUSED(request);
    _checking = false;
}



void ChatsModel::_removeChat(int id)
{
    if (!_ids.contains(id))
        return;

    _ids.remove(id);

    int i;
    for (i = 0; i < _allChats.size(); i++)
        if (_allChats.at(i)->id() == id)
        {
            _allChats.removeAt(i);
            break;
        }

    for (i = 0; i < _chats.size(); i++)
        if (_chats.at(i)->id() == id)
            break;
        
    if (i >= _chats.size())
        return;
    
    beginRemoveRows(QModelIndex(), i, i);
    
    _chats.removeAt(i);
    
    endRemoveRows();
}



void ChatsModel::_checkUnread(int actual)
{
    int found = 0;
    QList<int> bubbleIds;
    for (int i = 0; i < _chats.size(); i++)
    {
        if (_chats.at(i)->unreadCount() <= 0)
            continue;

        if (i > actual)
            bubbleIds << _chats.at(i)->id();

        found++;
        if (found >= actual)
            break;
    }

    foreach (auto id, bubbleIds)
        _bubbleChat(id);

    if (found < actual)
        loadUnread();
}



void ChatsModel::_bubbleChat(int id)
{
    if (!_ids.contains(id))
        return;

    int i = 0;
    for (; i < _chats.size(); i++)
        if (id == _chats.at(i)->id())
            break;

    if (i >= _chats.size())
        return;

    _chats.at(i)->update(); //! \todo why?

    int unread = 0;
    for (; unread < _chats.size(); unread++)
        if (_chats.at(unread)->unreadCount() <= 0) //! \todo newly created
            break;

    if (i <= unread)
        return;

    if (!beginMoveRows(QModelIndex(), i, i, QModelIndex(), unread))
        return;

    auto chat = _chats.takeAt(i);
    _chats.insert(unread, chat);

    endMoveRows();
}



bool ChatsModel::loading() const
{
    return _loading;
}
