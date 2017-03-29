// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "commentsmodel.h"

#include <QDateTime>
#include <QDebug>

#include "../defines.h"

#include "../data/Comment.h"
#include "../data/Notification.h"
#include "../data/Conversation.h"
#include "../data/Entry.h"

#include "../apirequest.h"

#include "notificationsmodel.h"



CommentsModel::CommentsModel(Entry *entry)
    : TastyListModel(entry)
    , _entryId(0)
    , _totalCount(0)
    , _url(QStringLiteral("v1/comments.json?entry_id=%1&limit=50"))
    , _entry(entry)
{
    if (!entry)
        return;

    _entryId = entry->entryId();

    _setTotalCount(entry->commentsCount());

    Q_TEST(connect(entry, SIGNAL(commentAdded(const QJsonObject)), this, SLOT(_addComment(const QJsonObject))));
        
    if (entry->chat())
        Q_TEST(connect(entry->chat(), SIGNAL(messageReceived(QJsonObject)), this, SLOT(check())));
}



void CommentsModel::init(const QJsonArray& feed, int totalCount)
{
    auto reset = _comments.size() != feed.size();
    if (reset)
        beginResetModel();

    _comments.clear();
    _ids.clear();

    _loadRequest = nullptr;
    _checkRequest = nullptr;

    auto cmts = _commentsList(feed);
    if (!cmts.isEmpty())
    {
        if (cmts.first()->createdDate() > cmts.last()->createdDate())
            for (auto it = cmts.rbegin(); it != cmts.rend(); ++it)
                _comments << *it;
        else
            _comments << cmts;
    }

    emit rowCountChanged();

    _totalCount = totalCount;

    if (reset)
        endResetModel();

    emit hasMoreChanged();
    emit lastCommentChanged();
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



bool CommentsModel::hasMore() const
{
    return _comments.size() < _totalCount;
}



Comment* CommentsModel::lastComment() const
{
    if (_comments.isEmpty())
        return nullptr;

    return _comments.last();
}



void CommentsModel::check()
{
    if (isChecking() || !_entryId || !_entry || _entry->isLoading())
        return;

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QStringLiteral("&from_comment_id=%1").arg(_comments.last()->id());

    _checkRequest = new ApiRequest(url);

    Q_TEST(connect(_checkRequest, SIGNAL(success(QJsonObject)),  this, SLOT(_addLastComments(QJsonObject))));

    _initCheck();
}



void CommentsModel::loadMore()
{
    if (isLoading() || !_entryId || !_entry || _entry->isLoading())// || !hasMore())
        return;

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QStringLiteral("&to_comment_id=%1").arg(_comments.first()->id());

    _loadRequest = new ApiRequest(url, ApiRequest::ShowMessageOnError);
    
    Q_TEST(connect(_loadRequest, SIGNAL(success(QJsonObject)),  this, SLOT(_addComments(QJsonObject))));

    _initLoad();
}



QHash<int, QByteArray> CommentsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "comment";
    return roles;
}



void CommentsModel::_addComments(const QJsonObject& data)
{
    auto feed = data.value(QLatin1String("comments")).toArray();
    if (feed.isEmpty())
    {
        _setTotalCount(_comments.size());
        emit hasMoreChanged();
        return;
    }

    _setTotalCount(data.value(QLatin1String("total_count")).toInt());

    _addComments(feed);
}



void CommentsModel::_addComments(const QJsonArray& feed)
{
    auto cmts = _commentsList(feed);
    if (cmts.isEmpty())
        return;

    beginInsertRows(QModelIndex(), 0, cmts.size() - 1);

    _comments = cmts + _comments;

    emit rowCountChanged();

    endInsertRows();

    if (_comments.size() <= feed.size())
        emit lastCommentChanged();

//    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();
}



void CommentsModel::_addLastComments(const QJsonObject& data)
{
    auto feed = data.value(QLatin1String("comments")).toArray();
    if (feed.isEmpty())
        return;

    auto cmts = _commentsList(feed);
    if (cmts.isEmpty())
        return;
    
    _setTotalCount(data.value(QLatin1String("total_count")).toInt());

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size() + cmts.size() - 1);

    _comments << cmts; 

    emit rowCountChanged();

    endInsertRows();

    emit lastCommentChanged();

//    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();
}



void CommentsModel::_addComment(const QJsonObject& data)
{
    auto cmt = new Comment(data, this);

    if (_ids.contains(cmt->id()))
    {
        delete cmt;
        return;
    }

    _setTotalCount(_totalCount + 1);

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size());

    _comments << cmt;
    _ids << cmt->id();

    emit rowCountChanged();

    Q_TEST(connect(cmt, SIGNAL(destroyed(QObject*)), this, SLOT(_removeComment(QObject*))));

    endInsertRows();

    emit lastCommentChanged();
}



void CommentsModel::_removeComment(QObject* cmt)
{
    auto comment = static_cast<Comment*>(cmt);
    
    auto i = _comments.indexOf(comment);
    if (i < 0)
        return;

    beginRemoveRows(QModelIndex(), i, i);

    _comments.removeAt(i);
    _ids.remove(comment->id());

    emit rowCountChanged();

    endRemoveRows();

    _setTotalCount(_totalCount - 1);

    if (i == _comments.size())
        emit lastCommentChanged();
}



void CommentsModel::_setTotalCount(int tc)
{
    if (tc == _totalCount)
        return;

    _totalCount = tc;
    emit totalCountChanged(_totalCount);
}



QList<Comment*> CommentsModel::_commentsList(const QJsonArray& feed)
{
    QList<Comment*> cmts;
    for (int i = 0; i < feed.size(); i++)
    {
        auto cmt = new Comment(feed.at(i).toObject(), this);
        if (_ids.contains(cmt->id()))
        {
            delete cmt;
            continue;
        }
        
        _ids << cmt->id();
        cmts.insert(i, cmt);

        Q_TEST(connect(cmt, SIGNAL(destroyed(QObject*)), this, SLOT(_removeComment(QObject*))));
    }
    
    return cmts;
}
