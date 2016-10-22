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

#include "tastylistmodel.h"

#include "../data/Conversation.h"

class Tasty;



class ChatsModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode     READ mode WRITE setMode  NOTIFY modeChanged)

public:
    enum Mode {
        AllChatsMode     = 0,
        PrivateChatsMode = 1,
        EntryChatsMode   = 2
    };

    Q_ENUMS(Mode)
    
    static ChatsModel* instance(Tasty* tasty = nullptr);

    ChatsModel(Tasty* tasty = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    void setMode(const Mode mode);
    Mode mode() const {return _mode; }

    void addChat(EntryPtr entry);

public slots:
    void loadUnread();
    void reset();

signals:
    void modeChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addUnread(QJsonArray data);
    void _addChats(QJsonArray data);
    void _removeChat(int id);
    void _checkUnread(int actual);

private:
    void _bubbleChat(int id);

    QList<ChatPtr>  _allChats;
    QList<ChatPtr>  _chats;
    QSet<int>       _ids;
    Mode            _mode;

    QString         _url;
    int             _page;
};

#endif // CHATSMODEL_H
