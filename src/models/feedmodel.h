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

#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>

#include "../data/Entry.h"

class ApiRequest;



class FeedModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode     mode        READ mode       WRITE setMode)
    Q_PROPERTY(int      tlog        READ tlog       WRITE setTlog)
    Q_PROPERTY(QString  slug        READ slug       WRITE setSlug)
    Q_PROPERTY(bool     hasMore     READ hasMore                    NOTIFY hasMoreChanged)
    Q_PROPERTY(bool     loading     READ loading                    NOTIFY loadingChanged)
    Q_PROPERTY(bool     isPrivate   READ isPrivate                  NOTIFY isPrivateChanged)
    Q_PROPERTY(int      minRating   READ minRating  WRITE setMinRating)
    Q_PROPERTY(QString  query       READ query      WRITE setQuery  NOTIFY queryChanged)
    Q_PROPERTY(QString  tag         READ tag        WRITE setTag    NOTIFY tagChanged)

public:
    enum Mode {
        InvalidMode,
        MyTlogMode,
        MyFavoritesMode,
        MyPrivateMode,
        FriendsMode,
        LiveMode,
        FlowsMode,
        AnonymousMode,
        BestMode,
        ExcellentMode,
        WellMode,
        GoodMode,
        BetterThanMode,
        TlogMode,
        FavoritesMode
    };

    Q_ENUMS(Mode)

    enum Privacy
    {
        Private,
        Public,
        Voting
    };

    Q_ENUMS(Privacy)

    FeedModel(QObject* parent = nullptr);
    ~FeedModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    void setMode(const Mode mode);
    Mode mode() const {return _mode; }

    void setTlog(const int tlog);
    int tlog() const {return _tlog; }

    void setSlug(const QString slug);
    QString slug() const { return _slug; }

    void setMinRating(const int rating);
    int minRating() const { return _minRating; }

    void setQuery(const QString query);
    QString query() const { return _query; }

    void setTag(const QString tag);
    QString tag() const { return _tag; }

    Q_INVOKABLE void reset(Mode mode = InvalidMode, int tlog = -1,
                           QString slug = QString(), QString query = QString(),
                           QString tag = QString());

    bool isPrivate() const { return _isPrivate; }

    bool hideMode() const;
    bool hideShort() const;
    bool hideNegative() const;
    bool loading() const;
    bool hasMore() const;

    void postText(const QString title, const QString content, Privacy privacy = Public);
    void postAnonymous(const QString title, const QString content);

    Q_INVOKABLE void setSinceEntryId(int id);
    Q_INVOKABLE void setSinceDate(const QString date);

signals:
    void hasMoreChanged();
    void loadingChanged();
    void isPrivateChanged();
    void queryChanged();
    void tagChanged();
    void entryCreated(EntryPtr entry);

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _addNewPost(QJsonObject data);
    void _changeHideSome();
    void _setPrivate(int errorCode);
    void _setNotLoading(QObject* request);
    void _resetOrReloadRatings();
    void _reloadRatings();
    void _setRatings(const QJsonArray data);
    
private:
    void _addAll(QList<EntryPtr>& all);
    bool _addSome(QList<EntryPtr>& all);
    void _clear();

    void _setUrl(Mode mode);

    QList<EntryPtr> _entries;
    QList<EntryPtr> _allEntries;
    QString _url;
    int _tlog;
    QString _slug;
    Mode _mode;
    bool _hasMore;
    bool _loading;
    int _lastEntry;
    bool _isPrivate;
    int _minRating;
    QString _query;
    QString _tag;
    int     _page;
    QString _prevDate;

    ApiRequest* _request;
};

#endif // FEEDMODEL_H
