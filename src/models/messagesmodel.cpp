#include "messagesmodel.h"

// #include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../data/Message.h"
#include "../data/Notification.h"
#include "../data/Conversation.h"
#include "../apirequest.h"
#include "notificationsmodel.h"



MessagesModel::MessagesModel(Message* last, Conversation* chat)
    : QAbstractListModel(chat)
    , _lastMessage(last)
    , _loading(false)
    , _url("v2/messenger/conversations/by_id/%1/messages.json?limit=20&order=desc")
{
    if (!chat)
        return;

    _chatId = chat->id();
    _totalCount = chat->totalCount();

     Q_TEST(connect(chat, SIGNAL(messageSent(QJsonObject)), this, SLOT(_addMessage(QJsonObject))));
    // Q_TEST(connect(NotificationsModel::instance(), SIGNAL(commentAdded(int,const Notification*)),
                                                // this, SLOT(_addComment(int,const Notification*))));
}



int MessagesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _messages.size();
}



QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _messages.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Message*>(_messages.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



void MessagesModel::setChatId(const int id)
{
    if (id > 0)
        _chatId = id;
}



void MessagesModel::check()
{
    if (_loading || !_chatId)
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_chatId);
    if (!_messages.isEmpty())
        url += QString("&from_message_id=%1").arg(_messages.last()->id());

    _request = new ApiRequest(url);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)),  this, SLOT(_addLastMessages(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)),   this, SLOT(_setNotLoading(QObject*))));
}



Message* MessagesModel::lastMessage() const
{
    if (_messages.isEmpty())
        return _lastMessage;

    return _messages.last();
}



void MessagesModel::loadMore()
{
    if (_loading || !_chatId)
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_chatId);
    if (!_messages.isEmpty())
        url += QString("&to_message_id=%1").arg(_messages.first()->id());

    _request = new ApiRequest(url);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)),  this, SLOT(_addMessages(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)),   this, SLOT(_setNotLoading(QObject*))));
}



QHash<int, QByteArray> MessagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "message";
    return roles;
}



void MessagesModel::_addMessages(const QJsonObject data)
{
    _loading = false;
    emit loadingChanged();
        
    auto msgs = data.value("messages").toArray();
    if (msgs.isEmpty())
    {
        _setTotalCount(_messages.size());
        emit hasMoreChanged();
        return;
    }

    beginInsertRows(QModelIndex(), 0, msgs.size() - 1);

    _setTotalCount(data.value("total_count").toInt());

    _messages.reserve(_messages.size() + msgs.size());

    for (int i = 0; i < msgs.size(); i++)
    {
        auto msg = new Message(msgs.at(i).toObject(), this);
        _messages.insert(i, msg);

        Q_TEST(connect(msg, SIGNAL(destroyed(QObject*)), this, SLOT(_removeMessage(QObject*))));
    }

    endInsertRows();

    if (_messages.size() >= _totalCount)
        emit hasMoreChanged();
}



void MessagesModel::_addLastMessages(const QJsonObject data)
{
    _loading = false;
    emit loadingChanged();

    auto msgs = data.value("messages").toArray();
    if (msgs.isEmpty())
        return;

    _setTotalCount(data.value("total_count").toInt());

    beginInsertRows(QModelIndex(), _messages.size(), _messages.size() + msgs.size() - 1);

    _messages.reserve(_messages.size() + msgs.size());

    for (int i = 0; i < msgs.size(); i++)
    {
        auto msg = new Message(msgs.at(i).toObject(), this);
        _messages << msg;

        Q_TEST(connect(msg, SIGNAL(destroyed(QObject*)), this, SLOT(_removeMessage(QObject*))));
    }

    endInsertRows();

    emit lastMessageChanged();
}



void MessagesModel::_addMessage(const QJsonObject data)
{
    _setTotalCount(_totalCount + 1);

    beginInsertRows(QModelIndex(), _messages.size(), _messages.size());

    auto msg = new Message(data, this);
    _messages << msg;

    Q_TEST(connect(msg, SIGNAL(destroyed(QObject*)), this, SLOT(_removeMessage(QObject*))));

    endInsertRows();

    emit lastMessageChanged();
}



void MessagesModel::_addMessage(const int chatId, const QJsonObject data)
{
    if (chatId != _chatId)
        return;

    // last ten comments must be enough
    // for (int i = _messages.size() - 1; i >= _messages.size() - 10 && i >= 0; i--)
        // if (_messages.at(i)->id() == notif->entityId())
            // return;

    _addMessage(data);
}



void MessagesModel::_removeMessage(QObject* msg)
{
    auto i = _messages.indexOf(static_cast<Message*>(msg));
    if (i < 0)
        return;

    beginRemoveRows(QModelIndex(), i, i);

    _messages.removeAt(i);

    endRemoveRows();

    _setTotalCount(_totalCount - 1);

    if (i == _messages.size())
        emit lastMessageChanged();
}



void MessagesModel::_setNotLoading(QObject* request)
{
    if (request == _request)
    {
        _loading = false;
        _request = nullptr;
    }
}



void MessagesModel::_setTotalCount(int tc)
{
    if (tc == _totalCount)
        return;

    _totalCount = tc;
    emit totalCountChanged(_totalCount);
}
