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

#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

#include "tastylistmodel.h"

class ApiRequest;
class Conversation;
class Message;

#ifdef Q_OS_ANDROID
class AndroidNotifier;
#endif



class MessagesModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(int chatId READ chatId)

    Q_PROPERTY(int size   READ rowCount NOTIFY rowCountChanged)

public:
    explicit MessagesModel(Conversation* chat = nullptr);

    void init(Conversation* chat);

    Q_INVOKABLE virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    int chatId() const { return _chatId; }

    virtual bool hasMore() const override;

    Message* lastMessage() const;

signals:
    void hasMoreChanged();
    void loadingChanged();
    void lastMessageChanged();

    void itemsAboutToBePrepended();
    void itemsPrepended(int prependedCount);

    void rowCountChanged();

public slots:
    void reset();
    void check();
    virtual void loadMore() override;

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addMessages(const QJsonObject& data);
    void _addLastMessages(const QJsonObject& data);
    void _addMessage(const QJsonObject& data);
    void _addMessage(const int chatId, const QJsonObject& data);
    void _removeMessage(QObject* msg);

private:
    QList<Message*> _messagesList(const QJsonArray& feed);

    QList<Message*> _messages;
    QSet<int>       _ids;
    Conversation*   _chat; //-V122
    
    int             _chatId;
    int             _totalCount;
    
    const QString   _url;

#ifdef Q_OS_ANDROID
    AndroidNotifier* _androidNotifier;
#endif
};

#endif // MESSAGESMODEL_H
