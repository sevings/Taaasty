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

#ifndef BAYES_H
#define BAYES_H

#include <QObject>
#include <QHash>
#include <QSqlDatabase>
#include <QReadWriteLock>

class EntryBase;
class Trainer;
class StemmerV;



class Bayes : public QObject
{
    Q_OBJECT

    friend class Trainer;

public:
    enum Type { Water, Fire, Unclassified };

    explicit Bayes(QObject *parent = 0);
    ~Bayes();

    static Bayes* instance(QObject* parent = nullptr);

    int classify(const EntryBase* entry, const int minLength = 0) const;
    int voteForEntry(const EntryBase* entry, const Type type);
    int entryVoteType(const int id) const;

    Trainer* trainer();
    QSqlDatabase& db() { return _db; }

public slots:
    void saveDb();

private:
    struct FeatureCount {
        FeatureCount(quint16 c = 0)
            : count(c), changed(false) { }
        FeatureCount& operator++()
        {
            count++;
            changed = true;
            return *this;
        }
        quint16 count;
        bool changed;
    };

    void _initDb();
    void _loadDb();

    bool _isEntryAdded(int id) const;

    int _calcText(QString text, QHash<QString, FeatureCount>& wordCounts) const;
    int _calcEntry(const EntryBase* entry, QHash<QString, FeatureCount>& wordCounts, const int minLength = 0) const;
    void _addEntry(const EntryBase* entry, const Type type);

    QHash<QString, FeatureCount> _wordCounts[Unclassified];
    QHash<int, bool>             _entriesChanged[Unclassified];
    int                          _total[Unclassified];

    bool _loaded;

    Trainer* _trainer; //-V122
    StemmerV* _stemmer; //-V122
    QSqlDatabase _db;
    mutable QReadWriteLock _lock;
};

#endif // BAYES_H
