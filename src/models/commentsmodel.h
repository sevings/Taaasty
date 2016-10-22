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

#ifndef COMMENTSMODEL_H
#define COMMENTSMODEL_H

#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

#include "tastylistmodel.h"

class Comment;
class Notification;
class Entry;



class CommentsModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(int entryId  READ entryId WRITE setEntryId)

public:
    explicit CommentsModel(Entry* entry = nullptr);

    void init(const QJsonArray feed, int totalCount);

    Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int entryId() const { return _entryId; }
    void setEntryId(const int id);

    virtual bool hasMore() const override;

    Comment* lastComment() const;

signals:
    void totalCountChanged(int tc);
    void lastCommentChanged();

public slots:
    void check();
    void loadMore();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addComments(const QJsonObject data);
    void _addComments(const QJsonArray feed);
    void _addLastComments(const QJsonObject data);
    void _addComment(const QJsonObject data);
    void _removeComment(QObject* cmt);

private:
    void            _setTotalCount(int tc);
    QList<Comment*> _commentsList(QJsonArray feed);

    QList<Comment*> _comments;
    QSet<int>       _ids;
    
    int             _entryId;
    int             _totalCount;
    
    const QString   _url;

    Entry*          _entry;
};

#endif // COMMENTSMODEL_H
