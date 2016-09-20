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

#include "Message.h"

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "../settings.h"
#include "../pusherclient.h"
#include "../models/chatsmodel.h"
#include "../models/attachedimagesmodel.h"

#include "User.h"
#include "Conversation.h"



Message::Message(QObject* parent)
    : MessageBase(parent)
    , _userId(0)
    , _recipientId(0)
    , _conversationId(0)
    , _replyTo(nullptr)
    , _replyUserId(0)
    , _chat(nullptr)
    , _attachedImagesModel(nullptr)
{
    _user = new User(this);
}



Message::Message(const QJsonObject data, Conversation* chat, QObject *parent)
    : MessageBase(parent)
    , _replyTo(nullptr)
    , _chat(chat)
    , _attachedImagesModel(nullptr)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));

    if (_userId != Tasty::instance()->settings()->userId())
        Q_TEST(connect(chat, SIGNAL(allMessagesRead(QJsonObject)), this, SLOT(_markRead(QJsonObject))));
    
    if (_user->slug().isEmpty())
        Q_TEST(connect(chat, SIGNAL(updated()), this, SLOT(_updateUser())));
}



Message::~Message()
{
    Tasty::instance()->pusher()->removeMessage(_id);
}



int Message::userId() const
{
    return _userId;
}



User* Message::replyTo()
{
    if (_replyUserId <= 0)
        return nullptr;

    if (!_replyTo)
        _replyTo = _chat->user(_replyUserId);

    return _replyTo;
}



void Message::read()
{
    if (_read || _id <= 0 || _userId == _chat->userId())
        return;

    auto url  = QString("v2/messenger/conversations/by_id/%1/messages/read.json").arg(_conversationId);
    auto data = QString("ids=%1").arg(_id);
    _request  = new ApiRequest(url, true, QNetworkAccessManager::PutOperation, data);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_markRead(QJsonObject))));
    
    _initRequest();
}



void Message::_init(const QJsonObject data)
{
    _id             = data.value("id").toInt();
    _userId         = data.value("user_id").toInt();
    _recipientId    = data.value("recipient_id").toInt();
    _conversationId = data.value("conversation_id").toInt();
    _read           = !data.value("read_at").isNull();
    auto d = data.value("created_at").toString();
    _createdAt      = Tasty::parseDate(d, _chat && _chat->type() == Conversation::PrivateConversation);
    _setDate(d);
    _text           = data.value("content_html").toString().replace("&amp;", "&"); // TODO: SystemMessage

    _user = _chat->user(_userId);
    Q_ASSERT(_user);
    
    auto imageAttach = data.value("attachments").toArray();
    delete _attachedImagesModel;
    if (imageAttach.isEmpty())
        _attachedImagesModel = nullptr;
    else
        _attachedImagesModel = new AttachedImagesModel(&imageAttach, this);
        
    _correctHtml();
    _setTruncatedText();

    auto reply = data.value("reply_message").toObject();
    _replyUserId = reply.value("user_id").toInt();

    Tasty::instance()->pusher()->addMessage(this);

    emit idChanged();
    emit userUpdated();
    emit readChanged();
    emit baseUpdated();
    emit updated();
}



void Message::_correctHtml()
{
    Tasty::correctHtml(_text, false);

    emit textUpdated();
}



void Message::_markRead(const QJsonObject data)
{
    if (data.value("status").toString() != "success")
    {
        qDebug() << "error read message" << _id;
        return;
    }

    _read = true;
    emit readChanged();
}



void Message::_updateUser()
{
    if (!_user->slug().isEmpty())
    {
        if (_chat)
        disconnect(_chat, SIGNAL(updated()), this, SLOT(_updateUser()));
        return;
    }
    
    _user = _chat->user(_userId);
    emit userUpdated();
    
    Q_ASSERT(_user);

    if (!_user->slug().isEmpty() && _chat)
        disconnect(_chat, SIGNAL(updated()), this, SLOT(_updateUser()));
}



void Message::_updateRead(const QJsonObject data)
{
    if (_read || data.value("id").toInt() != _id)
        return;

    _read = !data.value("read_at").isNull();
    emit readChanged();
}



void Message::_markRemoved(const QJsonObject data)
{
    if (data.value("id").toInt() != _id)
        return;

    _text = data.value("content").toString();
//    _type = data.value("type").toString();
    _setTruncatedText();

    emit textUpdated();
}
