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

#include "Tlog.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../tastydatacache.h"

#include "../models/chatsmodel.h"

#include "Author.h"
#include "Flow.h"



Tlog::Tlog(QObject* parent)
    : TastyData(parent)
    , _myRelation(Undefined)
    , _hisRelation(Undefined)
    , _author(new Author(this))
    , _flow(nullptr)
{

}



Tlog::Tlog(const QJsonObject& data, QObject *parent)
    : TastyData(parent)
    , _author(nullptr)
    , _flow(nullptr)
{
    init(data);
}



Conversation* Tlog::chat()
{
    if (_chat)
        return _chat.data();

    if (_id > 0)
    {
        _chat = pTasty->dataCache()->chatByTlog(_id);
        if (_chat)
            return _chat.data();
    }

    _chat = ChatPtr::create(nullptr);

    if (_id > 0)
    {
        _chat->setRecipientId(_id);
//        ChatsModel::instance()->addChat(_chat);
    }

    return _chat.data();
}



bool Tlog::changingRelation() const
{
    return _relationRequest;
}



void Tlog::setId(const int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    if (_chat)
        _chat->setRecipientId(_id);

    _slug.clear();

    reload();
}



void Tlog::setSlug(const QString& slug)
{
    if (slug.isEmpty() || slug == _slug)
        return;

   _slug = slug;
   _id = 0;
   emit idChanged();

   reload();
}



void Tlog::init(const QJsonObject& data)
{
    _id = data.value(QStringLiteral("id")).toInt();
    _slug = data.value(QStringLiteral("slug")).toString();
    _title = data.value(QStringLiteral("title")).toString();
    _tag = data.value(QStringLiteral("tag")).toString();
    _entriesCount = "<h1>" + Tasty::num2str(data.value(QStringLiteral("total_entries_count")).toInt(),
                                            "</h1>запись", "</h1>записи", "</h1>записей");
    _publicEntriesCount = "<h1>" + Tasty::num2str(data.value(QStringLiteral("public_entries_count")).toInt(),
                                                  "</h1>запись", "</h1>записи", "</h1>записей");
    _privateEntriesCount = "<h1>" + Tasty::num2str(data.value(QStringLiteral("private_entries_count")).toInt(),
                                                   "</h1>скрыта", "</h1>скрыто", "</h1>скрыто");

    if (data.contains(QStringLiteral("stats")))
    {
        auto stats = data.value(QStringLiteral("stats")).toObject();
        _favoritesCount = QStringLiteral("<h1>%1</h1>в избранном")
                .arg(stats.value(QStringLiteral("favorites_count")).toInt());
        _commentsCount = Tasty::num2str(stats.value(QStringLiteral("comments_count")).toInt(),
                                        "комментарий", "комментария", "комментариев");
        _tagsCount = "<h1>" + Tasty::num2str(stats.value(QStringLiteral("tags_count")).toInt(),
                                    "</h1>тег", "</h1>тега", "</h1>тегов");
        _daysCount = Tasty::num2str(stats.value(QStringLiteral("days_count")).toInt(),
                                    "день на Тейсти", "дня на Тейсти", "дней на Тейсти");
    }
    else
    {
        _favoritesCount = "Избранное";
        _commentsCount.clear();
        _tagsCount = "Теги";

        auto date = QDateTime::fromString(data.value(QStringLiteral("created_at")).toString().left(19), "yyyy-MM-ddTHH:mm:ss");
        auto today = QDateTime::currentDateTime();
        int days = (today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000);
        _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");
    }

    auto relations = data.value(QStringLiteral("relationships_summary")).toObject();
    _followersCount = "<h1>" + Tasty::num2str(relations.value(QStringLiteral("followers_count")).toInt(),
                                              "</h1>подписчик", "</h1>подписчика", "</h1>подписчиков");
    _followingsCount = "<h1>" + Tasty::num2str(relations.value(QStringLiteral("followings_count")).toInt(),
                                               "</h1>подписка", "</h1>подписки", "</h1>подписок");
    _ignoredCount = "<h1>" + Tasty::num2str(relations.value(QStringLiteral("ignored_count")).toInt(),
                                            "</h1>блокирован", "</h1>блокировано", "</h1>блокировано");

    _hisRelation = _relationship(data, "his_relationship");
    _myRelation =  _relationship(data, "my_relationship");

    auto authorData = data.contains(QStringLiteral("author"))
            ? data.value(QStringLiteral("author")).toObject() : data;
    if (_author)
        _author->init(authorData);
    else
        _author = new Author(authorData, this);

    auto myRelationObj = data.value(QStringLiteral("my_relationship_object")).toObject();
    if (myRelationObj.contains(QStringLiteral("flow")))
    {
        if (!_flow)
            _flow = new Flow(this);
        _flow->init(myRelationObj.value(QStringLiteral("flow")).toObject());
    }
    
    if (_chat)
        _chat->setRecipientId(_id);

    emit idChanged();
    emit updated();
    emit myRelationChanged();
    emit hisRelationChanged();
}



void Tlog::reload()
{
    if ((_slug.isEmpty() && !_id) || isLoading())
            return;

    auto url = QStringLiteral("v1/tlog/%1.json");
    if (_id)
        url = url.arg(_id);
    else
        url = url.arg(_slug);

    _request = new ApiRequest(url);
    _request->get();
    
    connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject)));

    _initRequest();
}



void Tlog::follow()
{
    Q_ASSERT(_myRelation != Friend && _myRelation != Requested);
    if (_myRelation == Friend || _myRelation == Requested)
        return;

    _changeMyRelation(QStringLiteral("v1/relationships/to/tlog/%1/follow.json"));
}



void Tlog::unfollow()
{
    Q_ASSERT(_myRelation == Friend || _myRelation == Requested);
    if (_myRelation != Friend && _myRelation != Requested)
        return;

    _changeMyRelation(QStringLiteral("v1/relationships/to/tlog/%1/unfollow.json"));
}



void Tlog::ignore()
{
    Q_ASSERT(_myRelation != Ignored);
    if (_myRelation == Ignored)
        return;

    _changeMyRelation(QStringLiteral("v1/relationships/to/tlog/%1/ignore.json"));
}



void Tlog::cancelIgnoring()
{
    Q_ASSERT(_myRelation == Ignored);
    if (_myRelation != Ignored)
        return;

    _changeMyRelation(QStringLiteral("v1/relationships/to/tlog/%1/cancel.json"));
}



void Tlog::unsubscribeHim()
{
    Q_ASSERT(_hisRelation == Friend);
    if (_hisRelation != Friend)
        return;

    if (_relationRequest || !Tasty::instance()->isAuthorized())
        return;

    auto url = QStringLiteral("v1/relationships/by/tlog/%1.json");
    _relationRequest = new ApiRequest(url.arg(_id), ApiRequest::AllOptions);
    _relationRequest->deleteResource();

    Q_TEST(connect(_relationRequest, SIGNAL(success(const QJsonObject)), this, SLOT(_setHisRelation(QJsonObject))));
    Q_TEST(connect(_relationRequest, &QObject::destroyed,
            this, &Tlog::changingRelationChanged, Qt::QueuedConnection));

    emit changingRelationChanged();
}



void Tlog::approveFriendRequest()
{
    _handleFriendRequest(QStringLiteral("v1/relationships/by/tlog/%1/approve.json"));
}



void Tlog::disapproveFriendRequest()
{
    _handleFriendRequest(QStringLiteral("v1/relationships/by/tlog/%1/disapprove.json"));
}



void Tlog::_setMyRelation(const QJsonObject& data)
{
    _myRelation = _relationship(data, "state");
    emit myRelationChanged();
}



void Tlog::_setHisRelation(const QJsonObject& data)
{
    _hisRelation = _relationship(data, "state");
    emit hisRelationChanged();
}



void Tlog::_changeMyRelation(const QString& url)
{
    if (_relationRequest || !Tasty::instance()->isAuthorized())
        return;

    _relationRequest = new ApiRequest(url.arg(_id), ApiRequest::AllOptions);
    _relationRequest->post();

    Q_TEST(connect(_relationRequest, SIGNAL(success(const QJsonObject)), this, SLOT(_setMyRelation(QJsonObject))));
    Q_TEST(connect(_relationRequest, &QObject::destroyed,
            this, &Tlog::changingRelationChanged, Qt::QueuedConnection));

    emit changingRelationChanged();
}



void Tlog::_handleFriendRequest(const QString& url)
{
    Q_ASSERT(_hisRelation == Requested);
    if (_hisRelation != Requested)
        return;

    if (_relationRequest || !pTasty->isAuthorized())
        return;

    _relationRequest = new ApiRequest(url.arg(_id), ApiRequest::AllOptions);
    _relationRequest->post();

    Q_TEST(connect(_relationRequest, SIGNAL(success(const QJsonObject)), this, SLOT(_setHisRelation(QJsonObject))));
    Q_TEST(connect(_relationRequest, &QObject::destroyed,
            this, &Tlog::changingRelationChanged, Qt::QueuedConnection));

    emit changingRelationChanged();
}



Tlog::Relationship Tlog::_relationship(const QJsonObject& data, const QString& field)
{
    if (!data.contains(field))
        return Undefined;

    auto relation = data.value(field).toString();
    if (relation == "friend")
        return Friend;
    if (relation == "requested")
        return Requested;
    if (relation == "none")
        return None;
    if (relation == "ignored")
        return Ignored;
    if (relation.isEmpty())
        return Me;

    qDebug() << "relation:" << relation;
    return Undefined;
}
