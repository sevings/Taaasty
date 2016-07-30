#include "commentsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../data/Comment.h"
#include "../data/Notification.h"
#include "../data/Conversation.h"
#include "../data/Entry.h"
#include "../apirequest.h"
#include "notificationsmodel.h"



CommentsModel::CommentsModel(Entry *entry)
    : QAbstractListModel(entry)
    , _loading(false)
    , _url("v1/comments.json?entry_id=%1&limit=20&order=desc")
{
    if (!entry)
        return;

    _entryId = entry->entryId();
    _totalCount = entry->commentsCount();

    Q_TEST(connect(entry, SIGNAL(commentAdded(QJsonObject)), this, SLOT(_addComment(QJsonObject))));

    if (entry->chat())
        Q_TEST(connect(entry->chat(), SIGNAL(messageReceived(QJsonObject)), this, SLOT(check())));
}



int CommentsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _comments.size();
}



QVariant CommentsModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _comments.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Comment*>(_comments.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



void CommentsModel::setEntryId(const int id)
{
    if (id > 0)
        _entryId = id;
}



void CommentsModel::check()
{
    if (_loading || !_entryId)
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QString("&from_comment_id=%1").arg(_comments.last()->_id);

    _request = new ApiRequest(url);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)),  this, SLOT(_addLastComments(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)),   this, SLOT(_setNotLoading(QObject*))));
}



Comment* CommentsModel::lastComment() const
{
    if (_comments.isEmpty())
        return nullptr;

    return _comments.last();
}



void CommentsModel::loadMore()
{
    if (_loading || !_entryId)// || !hasMore())
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QString("&to_comment_id=%1").arg(_comments.first()->_id);

    _request = new ApiRequest(url);
    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)),  this, SLOT(_addComments(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)),   this, SLOT(_setNotLoading(QObject*))));
}



QHash<int, QByteArray> CommentsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "comment";
    return roles;
}



void CommentsModel::_addComments(const QJsonObject data)
{
    auto feed = data.value("comments").toArray();
    if (feed.isEmpty())
    {
        _setTotalCount(_comments.size());
        emit hasMoreChanged();
        _loading = false;
        emit loadingChanged();
        return;
    }

    beginInsertRows(QModelIndex(), 0, feed.size() - 1);

    _setTotalCount(data.value("total_count").toInt());

    _comments.reserve(_comments.size() + feed.size());

    for (int i = 0; i < feed.size(); i++)
    {
        auto cmt = new Comment(feed.at(i).toObject(), this);
        _comments.insert(i, cmt);

        Q_TEST(connect(cmt, SIGNAL(destroyed(QObject*)), this, SLOT(_removeComment(QObject*))));
    }

    endInsertRows();

    if (_comments.size() <= feed.size())
        emit lastCommentChanged();

    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();

    _loading = false;
    emit loadingChanged();
}



void CommentsModel::_addLastComments(const QJsonObject data)
{
    _loading = false;
    emit loadingChanged();

    auto feed = data.value("comments").toArray();
    if (feed.isEmpty())
        return;

    _setTotalCount(data.value("total_count").toInt());

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size() + feed.size() - 1);

    _comments.reserve(_comments.size() + feed.size());

    for (int i = 0; i < feed.size(); i++)
    {
        auto cmt = new Comment(feed.at(i).toObject(), this);
        _comments << cmt;

        Q_TEST(connect(cmt, SIGNAL(destroyed(QObject*)), this, SLOT(_removeComment(QObject*))));
    }

    endInsertRows();

    emit lastCommentChanged();

    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();
}



void CommentsModel::_addComment(const QJsonObject data)
{
    auto cmt = new Comment(data, this);

    // last ten comments must be enough
    for (int i = _comments.size() - 1; i >= _comments.size() - 10 && i >= 0; i--)
        if (_comments.at(i)->_id == cmt->id())
        {
            delete cmt;
            return;
        }

    _setTotalCount(_totalCount + 1);

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size());

    _comments << cmt;

    Q_TEST(connect(cmt, SIGNAL(destroyed(QObject*)), this, SLOT(_removeComment(QObject*))));

    endInsertRows();

    emit lastCommentChanged();
}



void CommentsModel::_addComment(const int entryId, const Notification* notif)
{
    if (entryId != _entryId)
        return;

    // last ten comments must be enough
    for (int i = _comments.size() - 1; i >= _comments.size() - 10 && i >= 0; i--)
        if (_comments.at(i)->_id == notif->entityId())
            return;

    _setTotalCount(_totalCount + 1);

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size());

    auto cmt = new Comment(notif, this);
    _comments << cmt;

    Q_TEST(connect(cmt, SIGNAL(destroyed(QObject*)), this, SLOT(_removeComment(QObject*))));

    endInsertRows();

    emit lastCommentChanged();
}



void CommentsModel::_removeComment(QObject* cmt)
{
    auto i = _comments.indexOf(static_cast<Comment*>(cmt));
    if (i < 0)
        return;

    beginRemoveRows(QModelIndex(), i, i);

    _comments.removeAt(i);

    endRemoveRows();

    _setTotalCount(_totalCount - 1);

    if (i == _comments.size())
        emit lastCommentChanged();
}



void CommentsModel::_setNotLoading(QObject* request)
{
    if (request == _request)
    {
        _loading = false;
        _request = nullptr;
    }
}



void CommentsModel::_setTotalCount(int tc)
{
    if (tc == _totalCount)
        return;

    _totalCount = tc;
    emit totalCountChanged(_totalCount);
}
