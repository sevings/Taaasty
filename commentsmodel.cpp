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
        return comment.value("id").toInt();
    case UserIdRole:
        return comment.value("user").toObject().value("id").toInt();
    case UserNameRole:
        return comment.value("user").toObject().value("name").toString();
    case UserUrlRole:
        return comment.value("user").toObject().value("tlog_url").toString();
    case Thumb64Role:
        return comment.value("user").toObject().value("userpic").toObject().value("thumb64_url").toString();
    case Thumb128Role:
        return comment.value("user").toObject().value("userpic").toObject().value("thumb128_url").toString();
    case SymbolRole:
        return comment.value("user").toObject().value("userpic").toObject().value("symbol").toString();
    case CommentHtmlRole:
        return comment.value("comment_html").toString();
    case CreatedAtRole:
        return QDateTime::fromString(comment.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    case EditableRole:
        return comment.value("can_edit").toBool();
    case DeletableRole:
        return comment.value("can_delete").toBool();
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

    roles[CommentIdRole]    = "commentId";
    roles[UserIdRole]       = "userId";
    roles[UserNameRole]     = "userName";
    roles[UserUrlRole]      = "userUrl";
    roles[Thumb64Role]      = "thumb64";
    roles[Thumb128Role]     = "thumb128";
    roles[SymbolRole]       = "symbol";
    roles[CommentHtmlRole]  = "commentHtml";
    roles[CreatedAtRole]    = "createdAt";
    roles[EditableRole]     = "editable";
    roles[DeletableRole]    = "deletable";

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

    QList<QJsonObject> comments;
    comments.reserve(feed.size());
    foreach(auto comment, feed)
        comments << comment.toObject();

    _comments = comments + _comments;

    endInsertRows();

    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();

    _loading = false;
}
