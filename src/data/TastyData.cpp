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

#include "TastyData.h"

#include <QDebug>

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



QString TastyData::errorString() const
{
    return _errorString;
}



void TastyData::_setErrorString(int errorCode, QString str)
{    
    qDebug() << "TastyData error code" << errorCode;

    _errorString = str;
    emit errorStringChanged();
}



void TastyData::_initRequest()
{
    emit loadingChanged();
    
    if (!_request)
        return;
    
    _errorString.clear();

    Q_TEST(connect(_request, &QObject::destroyed, 
            this, &TastyData::loadingChanged, Qt::QueuedConnection));

    Q_TEST(connect(_request, SIGNAL(error(int,QString)),
                   this, SLOT(_setErrorString(int,QString))));
}
