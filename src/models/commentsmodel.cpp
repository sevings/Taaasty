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
    : QAbstractListModel(entry)
    , _entryId(0)
    , _loading(false)
    , _checking(false)
    , _totalCount(0)
    , _url("v1/comments.json?entry_id=%1&limit=50")
    , _request(nullptr)
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



void CommentsModel::init(const QJsonArray feed, int totalCount)
{
    auto reset = _comments.size() != feed.size();
    if (reset)
        beginResetModel();

    _comments.clear();
    _ids.clear();
    _loading = false;
    _checking = false;

    delete _request;
    _request = nullptr;

    auto cmts = _commentsList(feed);
    if (!cmts.isEmpty())
    {
        if (cmts.first()->createdDate() > cmts.last()->createdDate())
            for (auto it = cmts.rbegin(); it != cmts.rend(); ++it)
                _comments << *it;
        else
            _comments << cmts;
    }

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



void CommentsModel::check()
{
    if (_checking || !_entryId || !_entry || _entry->isLoading())
        return;

    _checking = true;

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QString("&from_comment_id=%1").arg(_comments.last()->id());

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
    if (_loading || !_entryId || !_entry || _entry->isLoading())// || !hasMore())
        return;

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_entryId);
    if (!_comments.isEmpty())
        url += QString("&to_comment_id=%1").arg(_comments.first()->id());

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
        _loading = false;
        emit loadingChanged();
        _setTotalCount(_comments.size());
        emit hasMoreChanged();
        return;
    }

    _setTotalCount(data.value("total_count").toInt());

    _addComments(feed);

    _loading = false;
    emit loadingChanged();
}



void CommentsModel::_addComments(const QJsonArray feed)
{
    auto cmts = _commentsList(feed);
    if (cmts.isEmpty())
    {
        _loading = false;
        emit loadingChanged();
        return;
    }

    beginInsertRows(QModelIndex(), 0, cmts.size() - 1);

    _comments = cmts + _comments;

    endInsertRows();

    if (_comments.size() <= feed.size())
        emit lastCommentChanged();

//    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();
}



void CommentsModel::_addLastComments(const QJsonObject data)
{
    _checking = false;

    auto feed = data.value("comments").toArray();
    if (feed.isEmpty())
        return;

    auto cmts = _commentsList(feed);
    if (cmts.isEmpty())
        return;
    
    _setTotalCount(data.value("total_count").toInt());

    beginInsertRows(QModelIndex(), _comments.size(), _comments.size() + cmts.size() - 1);

    _comments << cmts; 
    
    endInsertRows();

    emit lastCommentChanged();

//    if (_comments.size() >= _totalCount)
        emit hasMoreChanged();
}



void CommentsModel::_addComment(const QJsonObject data)
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

    endRemoveRows();

    _setTotalCount(_totalCount - 1);

    if (i == _comments.size())
        emit lastCommentChanged();
}



void CommentsModel::_setNotLoading(QObject* request)
{
    if (request != _request)
        return;

    if (_loading)
    {
        _loading = false;
        emit loadingChanged();
    }

    _checking = false;
    _request = nullptr;
}



void CommentsModel::_setTotalCount(int tc)
{
    if (tc == _totalCount)
        return;

    _totalCount = tc;
    emit totalCountChanged(_totalCount);
}



QList<Comment*> CommentsModel::_commentsList(QJsonArray feed)
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
