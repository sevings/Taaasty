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
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QHash>

#include "../data/Conversation.h"

class ApiRequest;



class ChatsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode     READ mode WRITE setMode  NOTIFY modeChanged)
    Q_PROPERTY(bool hasMore  READ hasMore             NOTIFY hasMoreChanged)
    Q_PROPERTY(bool loading  READ loading             NOTIFY loadingChanged)
    Q_PROPERTY(bool checking READ checking            NOTIFY checkingChanged)

public:
    enum Mode {
        AllChatsMode     = 0,
        PrivateChatsMode = 1,
        EntryChatsMode   = 2
    };

    Q_ENUMS(Mode)
    
    static ChatsModel* instance(QObject* parent = nullptr);

    ChatsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE bool hasMore() const { return _hasMore; }

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

    void addChat(EntryPtr entry);

    bool loading() const;
    bool checking() const;

public slots:
    void loadUnread();
    void reset();

signals:
    void hasMoreChanged();
    void loadingChanged();
    void checkingChanged();
    void modeChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addUnread(QJsonArray data);
    void _addChats(QJsonArray data);
    void _setNotLoading(QObject* request);
    void _setNotChecking(QObject* request);
    void _removeChat(int id);
    void _checkUnread(int actual);

private:
    void _bubbleChat(int id);

    QList<ChatPtr>       _allChats;
    QList<ChatPtr>       _chats;
    QSet<int>            _ids;
    Mode                 _mode;

    bool    _hasMore;
    
    QString _url;
    bool    _loading;
    bool    _checking;
    int     _page;

    ApiRequest* _request;
};

#endif // CHATSMODEL_H
