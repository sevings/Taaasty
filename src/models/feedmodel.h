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

#include <QJsonObject>
#include <QJsonArray>

#include "../defines.h"

#include "tastylistmodel.h"
#include "../data/Entry.h"

class ApiRequest;
class Tlog;



class FeedModel : public TastyListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode     mode        READ mode       WRITE setMode       NOTIFY modeChanged)
    Q_PROPERTY(Tlog*    tlog        READ tlog                           CONSTANT)
    Q_PROPERTY(int      tlogId      READ tlogId     WRITE setTlogId     NOTIFY tlogIdChanged)
    Q_PROPERTY(QString  slug        READ slug       WRITE setSlug       NOTIFY slugChanged)
    Q_PROPERTY(int      minRating   READ minRating  WRITE setMinRating  NOTIFY minRatingChanged )
    Q_PROPERTY(QString  query       READ query      WRITE setQuery      NOTIFY queryChanged)
    Q_PROPERTY(QString  tag         READ tag        WRITE setTag        NOTIFY tagChanged)
    Q_PROPERTY(bool     showFixed   READ showFixed                      NOTIFY modeChanged)

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

    FeedModel(QObject* parent = nullptr);
    ~FeedModel();

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool canFetchMore(const QModelIndex& parent) const override;
    virtual void fetchMore(const QModelIndex& parent) override;

    void setMode(const Mode mode);
    Mode mode() const {return _mode; }

    Tlog* tlog() const { return _tlog; }

    void setTlogId(const int tlogId);
    int tlogId() const;

    void setSlug(const QString& slug);
    QString slug() const;

    void setMinRating(const int rating);
    int minRating() const { return _minRating; }

    void setQuery(const QString& query);
    QString query() const { return _query; }

    void setTag(const QString& tag);
    QString tag() const { return _tag; }

    Q_INVOKABLE void reset(Mode mode = InvalidMode, int tlogId = -1,
                           const QString& slug = QString(),
                           const QString& query = QString(),
                           const QString& tag = QString());

    bool hideMode() const;
    bool hideShort() const;
    bool hideNegative() const;
    bool showFixed() const;
    bool loading() const;

    Q_INVOKABLE bool isRepostable(int entryId) const;
    Q_INVOKABLE bool isUnrepostable(int etryId) const;

    Q_INVOKABLE void setSinceEntryId(int id);
    Q_INVOKABLE void setSinceDate(const QString& date);

signals:
    void modeChanged();
    void tlogIdChanged();
    void slugChanged();
    void minRatingChanged();
    void queryChanged();
    void tagChanged();

public slots:
    void repost(int entryId);
    void unrepost(int entryId);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(const QJsonObject& data);
    void _changeHideSome();
    void _resetOrReloadRatings();
    void _reloadRatings();
    void _setRatings(const QJsonArray& data);

    void _addRepost(const QJsonObject& data);
    void _removeRepost(const QJsonObject& data);

    void _prependEntry(int id, int tlogId);
    void _removeEntry(int id);

private:
    void _addAll(QList<EntryPtr>& all, int& from);
    bool _addSome(QList<EntryPtr>& all, int& from, int& allFrom);
    void _prepend(const EntryPtr& entry);
    void _clear();

    void _setUrl(Mode mode);

    QList<EntryPtr>      _entries;
    QList<EntryPtr>      _allEntries;
    QHash<int, EntryPtr> _idEntries;
    int                  _fixedCount;
    int                  _allFixedCount;

    QString              _url;
    Tlog*                _tlog; //-V122
    Mode                 _mode;
    int                  _lastEntry;
    int                  _minRating;
    QString              _query;
    QString              _tag;
    int                  _page;
    QString              _prevDate;

    ApiRequestPtr   _repostRequest;
};

#endif // FEEDMODEL_H
