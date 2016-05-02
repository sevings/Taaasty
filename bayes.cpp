#include "bayes.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>



Bayes::Bayes(QObject *parent)
    : QObject(parent)
    , _loaded(false)
    , _lastFavorite(0)
{
    _total[Water] = 0;
    _total[Fire]  = 0;

    _loadDb();
}



Bayes *Bayes::instance()
{
    static auto bayes = new Bayes;
    return bayes;
}



void Bayes::_initDb()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bayes");
    Q_ASSERT(db.open());

    QSqlQuery query;
    Q_ASSERT(query.exec("CREATE TABLE IF NOT EXISTS bayes         (type INTEGER, word TEXT, total INTEGER, PRIMARY KEY(type, word))"));
    Q_ASSERT(query.exec("CREATE TABLE IF NOT EXISTS bayes_tlogs   (type INTEGER, n INTEGER, tlog INTEGER, latest INTEGER, PRIMARY KEY(tlog))"));
    Q_ASSERT(query.exec("CREATE TABLE IF NOT EXISTS bayes_entries (type INTEGER, entry INTEGER, PRIMARY KEY(entry))"));
}



void Bayes::_loadDb()
{
    if (_loaded)
        return;

    _initDb();

    QSqlQuery query;
    Q_ASSERT(query.exec("SELECT type, word, total FROM bayes"));
    while (query.next())
        _wordCounts[query.value(0).toInt()][query.value(1).toString()]
                = FeatureCount(query.value(2).toInt());

    Q_ASSERT(query.exec("SELECT type, sum(total) AS \"total\" FROM bayes GROUP BY type"));
    while (query.next())
        _total[query.value(0).toInt()] = query.value(1).toInt();

    Q_ASSERT(query.exec("SELECT type, tlog, latest, n FROM bayes_tlogs WHERE tlog > 0 ORDER BY n"));
    while (query.next())
        _tlogs[query.value(0).toInt()] << BayesTlog(query.value(1).toInt(),
                                                    query.value(2).toInt());

    Q_ASSERT(query.exec("SELECT latest FROM bayes_tlogs WHERE tlog = -1"));
    if (query.next())
        _lastFavorite = query.value(0).toInt();

    Q_ASSERT(query.exec("SELECT type, entry FROM bayes_entries"));
    while (query.next())
        _entriesChanged[query.value(0).toInt()][query.value(1).toInt()] = false;

    _loaded = true;
}
