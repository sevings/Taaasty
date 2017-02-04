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

#include "bayes.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QtMath>
#include <QStandardPaths>
#include <QtConcurrent>

#ifdef QT_DEBUG
#   include <QDateTime>
#endif

#include "../defines.h"

#include "../data/Entry.h"
#include "../data/Author.h"
#include "trainer.h"
#include "stemmerv.h"



Bayes::Bayes(QObject *parent)
    : QObject(parent)
    , _loaded(false)
    , _trainer(new Trainer(this))
    , _stemmer(StemmerV::instance())
    , _imgRe("<\\s*(?:a|img)[^>]+(?:href|src)\\s*=\\s*['\"]([^\\s'\"]+)['\"][^>]*>",
             QRegularExpression::CaseInsensitiveOption)
    , _tagRe(QStringLiteral("<[^>]*>"))
    , _htmlSeqRe(QStringLiteral("&\\w+;"))
    , _linkExtRe(QStringLiteral("https?:\\/\\/([\\w\\-\\.]+)\\.\\w+\\/[^\\s]*\\.(\\w+)\\s"))
    , _linkRe(QStringLiteral("https?:\\/\\/([\\w\\-\\.]+)\\.\\w+\\/?[^\\s]*\\s"))
    , _punctRe(QStringLiteral("[\\.!?]"))
    , _caseRe1(QStringLiteral("^\\s*[A-ZА-ЯЁ]"))
    , _caseRe2(QStringLiteral("[\\.!?]\\s*[A-ZА-ЯЁ]"))
    , _spaceRe(QStringLiteral("[A-ZА-ЯЁa-zа-яё][\\.,!?]\\s+[A-ZА-ЯЁa-zа-яё]"))
{
    qDebug() << "Bayes";

    _total[Water] = 0;
    _total[Fire]  = 0;

    _imgRe.optimize();
    _tagRe.optimize();
    _htmlSeqRe.optimize();
    _linkExtRe.optimize();
    _linkRe.optimize();
    _punctRe.optimize();
    _caseRe1.optimize();
    _caseRe2.optimize();
    _spaceRe.optimize();

    QtConcurrent::run(this, &Bayes::_loadDb);
}



Bayes::~Bayes()
{
    saveDb();
}



Bayes* Bayes::instance(QObject *parent)
{
    static auto bayes = new Bayes(parent);
    return bayes;
}



int Bayes::classify(const EntryBase* entry, const int minLength) const
{
    QReadLocker locker(&_lock);

    if (!_total[Water] || !_total[Fire])
        return 0;

    QHash<QString, Bayes::FeatureCount> features;
    int length = _calcEntry(entry, features, minLength);
    if (length <= 0)
        return length;

    double wordValues[Unclassified];
    for (int type = 0; type < Unclassified; type++)
    {
        auto k = (_total[Water] + _total[Fire]) / (double) _total[type]; //-V2005

        wordValues[type] = 0;
        foreach (auto word, features.keys())
        {
            int cnt = _wordCounts[type][word].count;
            if (cnt == 0)
                continue;

            auto value = qLn(cnt * k);
            wordValues[type] += value;
        }
    }

    auto result = (wordValues[Fire] - wordValues[Water]) / length * 50;
    return qRound(result);
}



int Bayes::voteForEntry(const EntryBase* entry, const Bayes::Type type)
{
    _addEntry(entry, type);
    return classify(entry);
}



int Bayes::entryVoteType(const int id) const
{
    if (_entriesChanged[Water].contains(id))
        return Bayes::Water;

    if (_entriesChanged[Fire].contains(id))
        return Bayes::Fire;

    return -1;
}



void Bayes::saveDb()
{
    if (!_loaded)
        return;

    QReadLocker locker(&_lock);

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    Q_TEST(_db.transaction());

    QSqlQuery query(_db);
    for (int type = 0; type < Unclassified; type++)
    {
        foreach (auto word, _wordCounts[type].keys())
            if (_wordCounts[type][word].changed)
            {
                Q_TEST(query.prepare(QStringLiteral("INSERT OR REPLACE INTO bayes VALUES (?, ?, ?)")));
                query.addBindValue(type);
                query.addBindValue(word);
                query.addBindValue(_wordCounts[type][word].count);
                Q_TEST(query.exec());

                _wordCounts[type][word].changed = false;
            }

        foreach (auto entry, _entriesChanged[type].keys())
            if (_entriesChanged[type][entry])
            {
                Q_TEST(query.prepare(QStringLiteral("INSERT OR REPLACE INTO bayes_entries VALUES (?, ?)")));
                query.addBindValue(type);
                query.addBindValue(entry);
                Q_TEST(query.exec());

                _entriesChanged[type][entry] = false;
            }
    }

    query.finish();

    Q_TEST(_db.commit());

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Bayes saved in" << ms << "ms";
#else
    qDebug() << "Bayes saved";
#endif
}



void Bayes::_initDb()
{
    _db = QSqlDatabase::addDatabase("QSQLITE", "bayes");
    _db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/bayes");
    Q_TEST(_db.open());

    QSqlQuery query(_db);
    Q_TEST(query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS bayes         (type INTEGER, word TEXT, total INTEGER, PRIMARY KEY(type, word))")));
    Q_TEST(query.exec(QStringLiteral("CREATE TABLE IF NOT EXISTS bayes_entries (type INTEGER, entry INTEGER, PRIMARY KEY(entry))")));
}



void Bayes::_loadDb()
{
    if (_loaded)
        return;

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    _initDb();

    QWriteLocker locker(&_lock);

    Q_TEST(_db.transaction());

    QSqlQuery query(_db);
    Q_TEST(query.exec(QStringLiteral("SELECT type, word, total FROM bayes")));
    while (query.next())
        _wordCounts[query.value(0).toInt()][query.value(1).toString()] //-V807
                = FeatureCount(query.value(2).toInt());

    Q_TEST(query.exec("SELECT type, sum(total) AS \"total\" FROM bayes GROUP BY type"));
    while (query.next())
        _total[query.value(0).toInt()] = query.value(1).toInt();

    Q_TEST(query.exec(QStringLiteral("SELECT type, entry FROM bayes_entries")));
    while (query.next())
        _entriesChanged[query.value(0).toInt()][query.value(1).toInt()] = false;

    query.finish();

    Q_TEST(_db.commit());

    _trainer->_loadDb();

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Bayes loaded in" << ms << "ms";
#else
    qDebug() << "Bayes loaded";
#endif

    _loaded = true;
}



bool Bayes::_isEntryAdded(int id) const
{
    QReadLocker locker(&_lock);

    return _entriesChanged[Water].contains(id)
            || _entriesChanged[Fire].contains(id);
}



int Bayes::_calcText(QString text, QHash<QString, Bayes::FeatureCount>& wordCounts) const
{
    if (text.isEmpty())
        return 0;

    text.replace(_imgRe, " \\1 ")
            .replace(_tagRe, " ")
            .replace(_htmlSeqRe, " ")
            .replace(_linkExtRe, " \\1 \\2 ")
            .replace(_linkRe, " \\1 ");

    int length = 0;
    if (text.contains(_punctRe))
    {
        length = 2;
        if (text.contains(_caseRe1) || text.contains(_caseRe2))
            ++wordCounts[".normal_case"];
        else
            ++wordCounts[".lower_case"];

        if (text.contains(_spaceRe))
            ++wordCounts[".correct_spaces"];
        else
            ++wordCounts[".wrong_spaces"];
    }

    auto words = _stemmer->stem(text);
    foreach (auto word, words)
        ++wordCounts[word];

    length += words.size();
    return length;
}



int Bayes::_calcEntry(const EntryBase* entry, QHash<QString, FeatureCount>& wordCounts, const int minLength) const
{
    int content = _calcText(entry->text(), wordCounts);
    int title   = _calcText(entry->title(), wordCounts);

    if (minLength > 0 && content < minLength && title < minLength)
        return -1;

    if (content > 100 || title > 100)
        ++wordCounts[".long"];
    else
        ++wordCounts[".short"];

    const auto author = entry->author();

    ++wordCounts[QStringLiteral(".type_%1").arg(entry->strType())];
    ++wordCounts[QStringLiteral(".author_%1").arg(author->slug())];

    if (author->isFemale())
        ++wordCounts[".female"];
    else
        ++wordCounts[".male"];

    if (author->isDaylog())
        ++wordCounts[".daylog"];
    else
        ++wordCounts[".wholelog"];

    if (author->isFlow())
        ++wordCounts[".flow"];
    else
        ++wordCounts[".tlog"];

    if (author->isPremium())
        ++wordCounts[".premium"];
    else
        ++wordCounts[".free"];

    return content + title + 7;
}



void Bayes::_addEntry(const EntryBase* entry, const Bayes::Type type)
{
    if (_isEntryAdded(entry->entryId()))
        return;

    QWriteLocker locker(&_lock);

    _entriesChanged[type][entry->entryId()] = true; //-V108
    _total[type] +=  _calcEntry(entry, _wordCounts[type]);
}



Trainer* Bayes::trainer()
{
    return _trainer;
}
