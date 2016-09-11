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

#ifndef STEMMERV_H
#define STEMMERV_H

#include <QRegularExpression>



class StemmerV
{
public:
    static StemmerV* instance();

    QString stemWord(const QString word);
    QStringList stem(const QString text);

private:
    StemmerV();

    QRegularExpression _pvre;
    QRegularExpression _reflexive;
    QRegularExpression _suffix;
    QRegularExpression _plural;
    QRegularExpression _derivational;
    QRegularExpression _perfectivePrefix;
    QRegularExpression _perfectiveSuffix;
    QRegularExpression _softSign;
    QRegularExpression _doubleN;

    QRegularExpression _separators;
};

#endif // STEMMERV_H
