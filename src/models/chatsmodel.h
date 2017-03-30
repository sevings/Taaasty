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

#ifndef CHATSMODEL_H
#define CHATSMODEL_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QHash>

#include "../defines.h"
#include "tastylistmodel.h"

#define pChats ChatsModel::instance()

class Tasty;
class StatusChecker;



class ChatsModel : public TastyListModel
{
    Q_OBJECT

public:
    static ChatsModel* instance(Tasty* tasty = nullptr);

    ChatsModel(Tasty* tasty = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    void addChat(const EntryPtr& entry);
    void addChat(const ChatPtr& chat);

public slots:
    void loadLast();
    void loadUnread();
    void reset();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addLast(const QJsonArray& data);
    void _addUnread(const QJsonArray& data);
    void _addChats(const QJsonArray& data);
    void _removeChat(int id);
    void _checkUnread(int actual);
    void _addChat();

private:
    void _insertEntryChat(const ChatPtr& chat);
    bool _insertChat(const ChatPtr& chat);
    QList<ChatPtr> _chatList(const QJsonArray& data);


    QList<ChatPtr>  _chats;
    QSet<int>       _ids;
    QHash<int, int> _entryChats;
    StatusChecker*  _statusChecker; //-V122

    QString         _url;
    int             _page;
};

#endif // CHATSMODEL_H
