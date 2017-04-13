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

#include "Message.h"

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "../settings.h"
#include "../tastydatacache.h"
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



Message::Message(const QJsonObject& data, Conversation* chat, QObject *parent)
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
        Q_TEST(connect(chat, &Conversation::usersUpdated, this, &Message::_updateUser));
}



Message::~Message()
{
    pTasty->dataCache()->removeMessage(_id);
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

    auto url  = QStringLiteral("v2/messenger/conversations/by_id/%1/messages/read.json").arg(_conversationId);
    _request  = new ApiRequest(url, ApiRequest::AccessTokenRequired);
    _request->addFormData("ids", _id);
    _request->put();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_markRead(QJsonObject))));

    _initRequest();
}



void Message::remove(bool forAll)
{
    if (isLoading())
        return;

    auto url = QStringLiteral("v2/messenger/conversations/by_id/%1/messages/delete_by_ids.json?ids=%2")
                .arg(_conversationId).arg(_id);
    if (forAll)
        url += QStringLiteral("&all=true");

    _request = new ApiRequest(url, ApiRequest::AllOptions);
    _request->deleteResource();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_remove(QJsonObject))));

    _initRequest();
}



void Message::_init(const QJsonObject& data)
{
    _id             = data.value(QLatin1String("id")).toInt();
    _userId         = data.value(QLatin1String("user_id")).toInt();
    _recipientId    = data.value(QLatin1String("recipient_id")).toInt();
    _conversationId = data.value(QLatin1String("conversation_id")).toInt();
    _read           = !data.value(QLatin1String("read_at")).isNull();
    auto d = data.value(QLatin1String("created_at")).toString();
    _createdAt      = Tasty::parseDate(d, _chat && _chat->type() == Conversation::PrivateConversation);
    _setDate(d);
    _text           = data.value(QLatin1String("content_html")).toString().replace("&amp;", "&"); //! \todo SystemMessage

    _user = _chat->user(_userId);
    Q_ASSERT(_user);

    _setType(data);

    auto imageAttach = data.value(QLatin1String("attachments")).toArray();
    delete _attachedImagesModel;
    if (_type == SystemMessage || imageAttach.isEmpty())
        _attachedImagesModel = nullptr;
    else
        _attachedImagesModel = new AttachedImagesModel(imageAttach, this);

    _containsImage = _attachedImagesModel;

    _correctHtml();
    _setTruncatedText();

    auto reply = data.value(QLatin1String("reply_message")).toObject();
    _replyUserId = reply.value(QLatin1String("user_id")).toInt();

    pTasty->dataCache()->addMessage(this);

    emit idChanged();
    emit userUpdated();
    emit readChanged(_read);
    emit baseUpdated();
    emit updated();
}



void Message::_correctHtml()
{
    Tasty::correctHtml(_text, false);

    emit textUpdated();
}



void Message::_markRead(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")).toString() != "success")
    {
        qDebug() << "error read message" << _id;
        return;
    }

    _read = true;
    emit readChanged(true);
}



void Message::_remove(const QJsonObject& data)
{
    if (data.value(QLatin1String("status")).toString() != "success")
    {
        qDebug() << "error remove message" << _id;
        return;
    }

    if (_type == SystemMessage)
        return;

    delete _attachedImagesModel;
    _attachedImagesModel = nullptr;
    _containsImage = false;

    _text = QStringLiteral("Сообщение удалено");
    _type = SystemMessage;
    _setTruncatedText();

    emit baseUpdated();
    emit updated();
    emit textUpdated();
}



void Message::_updateUser()
{
    if (!_chat)
        return;

    if (!_user->slug().isEmpty())
    {
        disconnect(_chat, &Conversation::usersUpdated, this, &Message::_updateUser);
        return;
    }

    _user = _chat->user(_userId);
    emit userUpdated();

    Q_ASSERT(_user);

    if (!_user->slug().isEmpty())
        disconnect(_chat, &Conversation::usersUpdated, this, &Message::_updateUser);
}



void Message::_updateRead(const QJsonObject& data)
{
    if (_read || data.value(QLatin1String("id")).toInt() != _id)
        return;

    _read = !data.value(QLatin1String("read_at")).isNull();
    emit readChanged(_read);
}



void Message::_markRemoved(const QJsonObject& data)
{
    if (data.value(QLatin1String("id")).toInt() != _id)
        return;

    delete _attachedImagesModel;
    _attachedImagesModel = nullptr;
    _containsImage = false;

    _text = data.value(QLatin1String("content")).toString();
    _setType(data);
    _setTruncatedText();

    emit baseUpdated();
    emit updated();
    emit textUpdated();
}



void Message::_setType(const QJsonObject& data)
{
    auto type = data.value(QLatin1String("type")).toString();
    if (type == "Message")
        _type = NormalMessage;
    else if (type == "SystemMessage")
        _type = SystemMessage;
    else
        _type = UnknownMessageType;

    Q_ASSERT(type.isEmpty() || _type != UnknownMessageType);
}
