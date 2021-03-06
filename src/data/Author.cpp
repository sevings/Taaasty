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

#include "Author.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"



Author::Author(QObject* parent)
    : User(parent)
    , _isFemale(false)
    , _isPrivacy(false)
    , _isOnline(false)
    , _isFlow(false)
    , _isPremium(false)
    , _isDaylog(false)
{

}



Author::Author(const QJsonObject& data, QObject *parent)
    : User(data, parent)
{
    init(data);
}



bool Author::isFemale() const
{
    return _isFemale;
}



bool Author::isFlow() const
{
    return _isFlow;
}



bool Author::isPremium() const
{
    return _isPremium;
}



bool Author::isDaylog() const
{
    return _isDaylog;
}



void Author::init(const QJsonObject& data)
{
    User::_init(data);

    _isFemale  = data.value(QLatin1String("is_female")).toBool();
    _isPrivacy = data.value(QLatin1String("is_privacy")).toBool();
    _isFlow    = data.value(QLatin1String("is_flow")).toBool();
    _isPremium = data.value(QLatin1String("is_premium")).toBool();
    _isDaylog  = data.value(QLatin1String("is_daylog")).toBool();
    _title     = data.value(QLatin1String("title")).toString();

    _entriesCount = Tasty::num2str(data.value(QLatin1String("total_entries_count")).toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value(QLatin1String("public_entries_count")).toInt(),
                                         "запись", "записи", "записей");
    _privateEntriesCount = Tasty::num2str(data.value(QLatin1String("private_entries_count")).toInt(),
                                          "скрытая запись", "скрытые записи", "скрытых записей");

    auto date = QDateTime::fromString(data.value(QLatin1String("created_at")).toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    auto today = QDateTime::currentDateTime();
    int days = qRound(double(today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000)); //-V2005
    _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");

    _followingsCount = Tasty::num2str(data.value(QLatin1String("followings_count")).toInt(), "подписка", "подписки", "подписок");

    emit authorUpdated();

    _initStatus(data);
}



void Author::checkStatus()
{
    auto url = QStringLiteral("v1/online_statuses.json?user_ids=%1").arg(id());
    auto request = new ApiRequest(url);
    request->get();

    Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_initStatus(QJsonArray))));
}



void Author::reload()
{
    if (id() <= 0 || isLoading())
        return;

    _request = new ApiRequest(QStringLiteral("v1/tlog/%1.json").arg(id()));
    _request->get();

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_initFromTlog(QJsonObject))));

    _initRequest();
}



void Author::_initFromTlog(const QJsonObject& data)
{
    auto author = data.value(QLatin1String("author")).toObject();
    init(author);
}



void Author::_initStatus(const QJsonArray& data)
{
    if (data.size())
        _initStatus(data.first().toObject());
}



void Author::_initStatus(const QJsonObject& data)
{
    auto online = data.value(QLatin1String("is_online")).toBool();
    auto last = data.value(QLatin1String("last_seen_at")).toString();
    if (online)
        last = "Онлайн";
    else if (last.isEmpty())
        last = "Не в сети";
    else
        last = QStringLiteral("Был%1 в сети %2").arg(_isFemale ? "а" : QString())
                .arg(Tasty::parseDate(last, false));

    if (online == _isOnline && last == _lastSeenAt)
        return;

    _isOnline = online;
    _lastSeenAt = last;

    emit statusUpdated();
}
