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

#ifndef POSTER_H
#define POSTER_H

#include <QObject>
#include <QPointer>

#include "defines.h"
#include "apirequest.h"



class Poster : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool loading         READ isLoading      NOTIFY loadingChanged)
    Q_PROPERTY(QString errorString  READ errorString    NOTIFY errorStringChanged)

public:    
    enum Privacy
    {
        Voting,
        Public,
        Private
    };

    Q_ENUMS(Privacy)

    explicit Poster(QObject* parent = 0);

    Entry*  entry() const;
    bool    isLoading() const;
    QString errorString() const;

signals:
    void posted(EntryPtr entry);
    void loadingChanged();
    void errorStringChanged();

public slots:    
    void postText(QString title, QString content, Privacy privacy, int tlogId = 0);
    void postAnonymous(QString title, QString content);

private slots:
    void _createPostedEntry(const QJsonObject& data);
    void _setErrorString(int errorCode, QString str);

private:
    void _prepareText(QString& title, QString& content) const;

    int                  _tlogId;
    EntryPtr             _entry;
    QPointer<ApiRequest> _request;
    QString              _errorString;
};

#endif // POSTER_H
