#include "commentsmodel.h"

#include <QDateTime>
#include <QJsonArray>
#include <QDebug>

#include "apirequest.h"



CommentsModel::CommentsModel(QObject *parent)
    : QAbstractListModel(parent)
    , _entryId(0)
    , _loading(false)
    , _toComment(0)
    , _totalCount(1)
    , _url("comments.json?entry_id=%1&limit=20&order=desc")
{
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

    const auto comment = _comments.at(index.row());
    switch (role)
    {
    case CommentIdRole:
        return comment->id();
    case UserIdRole:
        return comment->user()->id();
    case UserNameRole:
        return comment->user()->name();
    case UserUrlRole:
        return comment->user()->tlogUrl();
    case Thumb64Role:
        return comment->user()->thumb64();
    case Thumb128Role:
        return comment->user()->thumb128();
    case SymbolRole:
        return comment->user()->symbol();
    case CommentHtmlRole:
        return comment->html();
    case CreatedAtRole:
        return comment->createdAt();
    case EditableRole:
        return comment->isEditable();
    case DeletableRole:
        return comment->isDeletable();
//    case CommentObjectRole:
//        return comment;
    }

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

    roles[CommentIdRole]     = "commentId";
    roles[UserIdRole]        = "userId";
    roles[UserNameRole]      = "userName";
    roles[UserUrlRole]       = "userUrl";
    roles[Thumb64Role]       = "thumb64";
    roles[Thumb128Role]      = "thumb128";
    roles[SymbolRole]        = "symbol";
    roles[CommentHtmlRole]   = "commentHtml";
    roles[CreatedAtRole]     = "createdAt";
    roles[EditableRole]      = "editable";
    roles[DeletableRole]     = "deletable";
    roles[CommentObjectRole] = "comment";

    return roles;
}

void CommentsModel::_addComments(QJsonObject data)
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
