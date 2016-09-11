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

#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

class ApiRequest;
class Conversation;
class Message;

#ifdef Q_OS_ANDROID
class AndroidNotifier;
#endif



class MessagesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int chatId           READ chatId)
    Q_PROPERTY(bool hasMore         READ hasMore    NOTIFY hasMoreChanged)
    Q_PROPERTY(bool loading         READ loading    NOTIFY loadingChanged)

public:
    explicit MessagesModel(Conversation* chat = nullptr);

    void init(Conversation* chat);

    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int chatId() const { return _chatId; }
    Q_INVOKABLE void reset();

    Q_INVOKABLE bool hasMore() const { return _messages.size() < _totalCount; }
    Q_INVOKABLE bool loading() const { return _loading; }

    Q_INVOKABLE void check();

    Message* lastMessage() const;

signals:
    void hasMoreChanged();
    void loadingChanged();
    void totalCountChanged(int tc);
    void lastMessageChanged();

public slots:
    void loadMore();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addMessages(const QJsonObject data);
    void _addLastMessages(const QJsonObject data);
    void _addMessage(const QJsonObject data);
    void _addMessage(const int chatId, const QJsonObject data);
    void _removeMessage(QObject* msg);
    void _setNotLoading(QObject* request);

private:
    void            _setTotalCount(int tc);
    QList<Message*> _messagesList(QJsonArray feed);

    QList<Message*> _messages;
    QSet<int>       _ids;
    Conversation*   _chat;
    
    int             _chatId;
    bool            _loading;
    int             _totalCount;
    
    const QString   _url;

    ApiRequest* _request;

#ifdef Q_OS_ANDROID
    AndroidNotifier* _androidNotifier;
#endif
};

#endif // MESSAGESMODEL_H
