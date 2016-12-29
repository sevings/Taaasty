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

#ifndef REPOSTER_H
#define REPOSTER_H

#include "defines.h"

class FeedModel;



class Reposter : public QObject
{
    Q_OBJECT
public:
    explicit Reposter(FeedModel* parent = 0);

    Q_INVOKABLE bool isRepostable(int entryId) const;
    Q_INVOKABLE bool isUnrepostable(int entryId) const;

signals:
    void reposted(const EntryPtr& entry);
    void unreposted(int entryId);

public slots:
    void repost(int entryId);
    void unrepost(int entryId);

private:
    int _tlogId() const;

    FeedModel*    _model;
    ApiRequestPtr _request;
    int           _entryId;
};

#endif // REPOSTER_H
