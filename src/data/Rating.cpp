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

#include "Rating.h"

#include <QtConcurrent>

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "../settings.h"

#include "Entry.h"
#include "Author.h"



Rating::Rating(QObject* parent)
    : TastyData(parent)
    , _votes(0)
    , _rating(0)
    , _isVoted(false)
    , _isVotable(false)
    , _bayesRating(0)
    , _isBayesVoted(false)
    , _isVotedAgainst(false)
    , _parent(qobject_cast<Entry*>(parent))
{    
    Q_TEST(connect(&_watcher, &QFutureWatcher<void>::finished, this, &Rating::bayesChanged, Qt::QueuedConnection));
}



Rating::Rating(const QJsonObject& data, Entry* parent)
    : TastyData(parent)
    , _bayesRating(0)
    , _isBayesVoted(false)
    , _isVotedAgainst(false)
    , _parent(parent)
{
    Q_TEST(connect(&_watcher, &QFutureWatcher<void>::finished, this, &Rating::bayesChanged, Qt::QueuedConnection));

    init(data);
}



Rating::~Rating()
{
    if (!_watcher.isFinished())
        _watcher.waitForFinished();
}



void Rating::reCalcBayes()
{
    auto type = Bayes::instance()->entryVoteType(_id);
    switch (type)
    {
    case Bayes::Water:
        _isBayesVoted = false;
        _isVotedAgainst = true;
        break;
    case Bayes::Fire:
        _isBayesVoted = true;
        _isVotedAgainst = false;
        break;
    default:
        _isBayesVoted = false;
        _isVotedAgainst = false;

        if (_isVoted)
        {
            voteBayes();
            return;
        }
    }

    _bayesRating = Bayes::instance()->classify(_parent);
    emit bayesChanged();
}



int Rating::bayesRating() const
{
    return _bayesRating;
}



void Rating::setId(int entryId)
{
    if (_id == entryId || isLoading())
        return;
    
    _id = entryId;
    
    auto url = QString("v1/ratings.json?ids=%1").arg(_id);
    _request = new ApiRequest(url, ApiRequest::ShowMessageOnError);
    
    Q_TEST(connect(_request, SIGNAL(success(QJsonArray)), this, SLOT(_reinit(QJsonArray))));
    
    _initRequest();
}



void Rating::vote()
{
    voteBayes();

    if (!_isVotable || !Tasty::instance()->isAuthorized())
        return;

    auto url = QString("v1/entries/%1/votes.json").arg(_id);
    auto operation = (_isVoted ? QNetworkAccessManager::DeleteOperation
                               : QNetworkAccessManager::PostOperation);
    auto request = new ApiRequest(url, ApiRequest::AccessTokenRequired | ApiRequest::ShowMessageOnError, operation);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)),
                   this, SLOT(init(const QJsonObject))));
}



void Rating::voteBayes()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    auto future = QtConcurrent::run(this, &Rating::_changeBayesRating, Bayes::Fire);
    _watcher.setFuture(future);

    _isBayesVoted = true;
}



void Rating::voteAgainst()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    auto future = QtConcurrent::run(this, &Rating::_changeBayesRating, Bayes::Water);
    _watcher.setFuture(future);

    _isVotedAgainst = true;
}



void Rating::init(const QJsonObject& data)
{
    int id = data.value("entry_id").toInt();
    if (id)
        _id = id;

    _votes      = data.value("votes").toInt();
    _rating     = data.value("rating").toInt();
    _isVoted    = data.value("is_voted").toBool();
    _isVotable  = data.value("is_voteable").toBool() && (!_parent || (_parent->isVotable() 
            && _parent->type() != "anonymous"
            && _parent->author()->id() != Tasty::instance()->settings()->userId()));

    emit dataChanged();
}



void Rating::_changeBayesRating(Bayes::Type type)
{
    _bayesRating = Bayes::instance()->voteForEntry(_parent, type);
}



void Rating::_reinit(const QJsonArray& data)
{
    if (data.isEmpty())
        return;
    
    init(data.first().toObject());

    reCalcBayes();
}
