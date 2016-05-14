#include "commentsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "defines.h"

#include "apirequest.h"
#include "notificationsmodel.h"



CommentsModel::CommentsModel(Entry *entry)
    : QAbstractListModel(entry)
    , _loading(false)
    , _url("comments.json?entry_id=%1&limit=20&order=desc")
{
    if (!entry)
        return;

    _entryId = entry->_id;
    _totalCount = entry->_commentsCount;

    Q_TEST(connect(entry, SIGNAL(commentAdded(QJsonObject)), this, SLOT(_addComment(QJsonObject))));
    Q_TEST(connect(NotificationsModel::instance(), SIGNAL(commentAdded(int,const Notification*)),
                                                this, SLOT(_addComment(int,const Notification*))));
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

    auto request = new ApiRequest(url);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addLastComments(QJsonObject)));
}



void CommentsModel::loadMore()
{
    if (_loading || !_entryId)
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QString("&to_comment_id=%1").arg(_comments.first()->_id);

    auto request = new ApiRequest(url);
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_addComments(QJsonObject)));
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
        _totalCount = _comments.size();
        emit hasMoreChanged();
        _loading = false;
        return;
    }

    _totalCount = data.value("total_count").toInt();

    beginInsertRows(QModelIndex(), 0, feed.size() - 1);

    _comments.reserve(_comments.size() + feed.size());

    for (int i = 0; i < feed.size(); i++)
        _comments.insert(i, new Comment(feed.at(i).toObject(), this));

    endInsertRows();

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

    beginInsertRows(QModelIndex(), _comments.size() - 1, _comments.size() + feed.size() - 1);

    _comments.reserve(_comments.size() + feed.size());

    for (int i = 0; i < feed.size(); i++)
        _comments << new Comment(feed.at(i).toObject(), this);

    endInsertRows();
}



void CommentsModel::_addComment(const QJsonObject data)
{
    beginInsertRows(QModelIndex(), _comments.size(), _comments.size());

    _comments << new Comment(data, this);

    endInsertRows();
}



void CommentsModel::_addComment(const int entryId, const Notification* notif)
{
    if (entryId != _entryId)
        return;

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size());

    _comments << new Comment(notif, this);

    endInsertRows();
}

