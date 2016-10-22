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

#include "TastyData.h"

#include "../defines.h"
#include "../apirequest.h"



TastyData::TastyData(QObject* parent)
    : QObject(parent)
    , _id(0)
{

}



int TastyData::id() const
{
    return _id;
}



bool TastyData::isLoading() const
{
    return _request;
}



void TastyData::_initRequest(bool emitting)
{
    if (emitting)
        emit loadingChanged();
    
    if (!_request)
        return;
    
    Q_TEST(connect(_request, &QObject::destroyed, 
            this, &TastyData::loadingChanged, Qt::QueuedConnection));
}
