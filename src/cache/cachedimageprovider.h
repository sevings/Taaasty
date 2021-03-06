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

#include <QQuickImageProvider>

#include "../defines.h"

class CacheManager;



class CachedImageProvider : public QQuickImageProvider
{
public:
    CachedImageProvider(CacheManager* man);

    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override;

    void setCacheSize(int kb);

private:
    CacheManager* _man; //-V122
};
