#include "commentsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "apirequest.h"



CommentsModel::CommentsModel(Entry *entry)
    : QAbstractListModel(entry)
    , _loading(false)
    , _toComment(0)
    , _url("comments.json?entry_id=%1&limit=20&order=desc")
{
    if (!entry)
        return;

    _entryId = entry->_id;
    _totalCount = entry->_commentsCount;

    connect(entry, SIGNAL(commentAdded(QJsonObject)), this, SLOT(_addComment(QJsonObject)));
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



void CommentsModel::loadMore()
{
    if (_loading || !_entryId)
        return;

    _loading = true;

    QString url = _url.arg(_entryId);
    if (_toComment)
        url += QString("&to_comment_id=%1").arg(_toComment);

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
        return;
    }

    _toComment = feed.first().toObject().value("id").toInt();
    _totalCount = data.value("total_count").toInt();

    beginInsertRows(QModelIndex(), 0, feed.size() - 1);

    QList<Comment*> comments;
    comments.reserve(feed.size());
    foreach(auto comment, feed)
        comments << new Comment(comment.toObject(), this);

    _comments = comments + _comments;

    endInsertRows();

    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();

    _loading = false;
}



void CommentsModel::_addComment(const QJsonObject data)
{
    beginInsertRows(QModelIndex(), _comments.size(), _comments.size());

    _comments << new Comment(data, this);

    endInsertRows();
}

