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

#pragma once

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QFutureWatcher>

#include "../nbc/bayes.h"
#include "TastyData.h"

class Entry;



class Rating: public TastyData
{
    Q_OBJECT

    Q_PROPERTY (int  entryId    MEMBER _id          NOTIFY dataChanged)
    Q_PROPERTY (int  votes      MEMBER _votes       NOTIFY dataChanged)
    Q_PROPERTY (int  rating     MEMBER _rating      NOTIFY dataChanged)
    Q_PROPERTY (bool isVoted    MEMBER _isVoted     NOTIFY dataChanged)
    Q_PROPERTY (bool isVotable  MEMBER _isVotable   NOTIFY dataChanged)

    Q_PROPERTY (int  bayesRating    MEMBER _bayesRating     NOTIFY bayesChanged)
    Q_PROPERTY (bool isBayesVoted   MEMBER _isBayesVoted    NOTIFY bayesChanged)
    Q_PROPERTY (bool isVotedAgainst MEMBER _isVotedAgainst  NOTIFY bayesChanged)

public:
    Rating(QObject* parent = nullptr);
    Rating(const QJsonObject data, Entry* parent = nullptr);
    ~Rating();

    void reCalcBayes();

    int bayesRating() const;

    void setId(int entryId);
    
public slots:
    void vote();
    void voteBayes();
    void voteAgainst();

    void init(const QJsonObject data);

signals:
    void dataChanged();
    void bayesChanged();

private slots:
    void _changeBayesRating(Bayes::Type type);
    void _reinit(const QJsonArray data);

private:
    int  _votes;
    int  _rating;
    bool _isVoted;
    bool _isVotable;

    int  _bayesRating;
    bool _isBayesVoted;
    bool _isVotedAgainst;

    Entry* _parent;

    QFutureWatcher<void> _watcher;
};
