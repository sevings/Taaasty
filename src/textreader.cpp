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

#include "textreader.h"

#include <QFile>
#include <QDebug>


TextReader::TextReader(QObject *parent)
    : QObject(parent)
{

}



QString TextReader::read()
{
    if (_source.isEmpty()){
        qDebug() << "text file source is empty";
        return QString();
    }

    QFile file(_source);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open the file" << _source << file.errorString();
        return QString();
    }

    QString line;
    QString fileContent;
    QTextStream t( &file );
    do {
        line = t.readLine();
        fileContent += line;
     } while (!line.isNull());

    file.close();

    return fileContent;
}
