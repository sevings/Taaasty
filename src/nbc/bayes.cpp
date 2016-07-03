#include "bayes.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QtMath>
#include <QStandardPaths>
#include <QTimer>

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
    , _trainer(nullptr)
    , _stemmer(StemmerV::instance())
    , _saveTimer(new QTimer(this))

{
    qDebug() << "Bayes";

    _total[Water] = 0;
    _total[Fire]  = 0;

    _saveTimer->setInterval(240000);
    _saveTimer->setSingleShot(false);
    _saveTimer->start();

    Q_TEST(connect(_saveTimer, SIGNAL(timeout()), this, SLOT(_saveDb())));

    _loadDb();
}



Bayes::~Bayes()
{
    _saveDb();
}



Bayes *Bayes::instance(QObject *parent)
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

    float wordValues[Unclassified];
//    float featureValues[Unclassified];
    for (int type = 0; type < Unclassified; type++)
    {
        auto k = (_total[Water] + _total[Fire]) / (float) _total[type];

        wordValues[type] = 0;
//        featureValues[type] = 0;
        foreach (auto word, features.keys())
        {
            int cnt = _wordCounts[type][word].count;
            if (cnt <= 0)
                continue;

            auto value = qLn(cnt * k);
//            if (word.startsWith('.'))
//                featureValues[type] += value;
//            else
                wordValues[type] += value;

        }
    }

    auto result = (wordValues[Fire] - wordValues[Water]) / length * 50;
//    result += (featureValues[Fire] - featureValues[Water]) / 5;
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



void Bayes::_saveDb()
{
    if (!_loaded)
        return;

    QReadLocker locker(&_lock);

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    Q_TEST(_db.transaction());

    QSqlQuery query;
    for (int type = 0; type < Unclassified; type++)
    {
        foreach (auto word, _wordCounts[type].keys())
            if (_wordCounts[type][word].changed)
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes VALUES (?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(word);
                query.addBindValue(_wordCounts[type][word].count);
                Q_TEST(query.exec());
                
                _wordCounts[type][word].changed = false;
            }

        foreach (auto entry, _entriesChanged[type].keys())
            if (_entriesChanged[type][entry])
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes_entries VALUES (?, ?)"));
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
    qDebug() << "Saved in" << ms << "ms";
#else
    qDebug() << "Bayes saved";
#endif
}



void Bayes::_initDb()
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/bayes");
    Q_TEST(_db.open());

    QSqlQuery query;
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes         (type INTEGER, word TEXT, total INTEGER, PRIMARY KEY(type, word))"));
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes_entries (type INTEGER, entry INTEGER, PRIMARY KEY(entry))"));
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

    QSqlQuery query;
    Q_TEST(query.exec("SELECT type, word, total FROM bayes"));
    while (query.next())
        _wordCounts[query.value(0).toInt()][query.value(1).toString()]
                = FeatureCount(query.value(2).toInt());

    Q_TEST(query.exec("SELECT type, sum(total) AS \"total\" FROM bayes GROUP BY type"));
    while (query.next())
        _total[query.value(0).toInt()] = query.value(1).toInt();

    Q_TEST(query.exec("SELECT type, entry FROM bayes_entries"));
    while (query.next())
        _entriesChanged[query.value(0).toInt()][query.value(1).toInt()] = false;

    query.finish();

    Q_TEST(_db.commit());

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Loaded in" << ms << "ms";
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

    text.replace(QRegularExpression("<\\s*(?:a|img)[^>]+(?:href|src)\\s*=\\s*['\"]([^\\s'\"]+)['\"][^>]*>",
                                    QRegularExpression::CaseInsensitiveOption), " \\1 ")
            .replace(QRegularExpression("<[^>]*>"), " ")
            .replace(QRegularExpression("&\\w+;"), " ")
            .replace(QRegularExpression("https?:\\/\\/([\\w\\-\\.]+)\\.\\w+\\/[^\\s]*\\.(\\w+)\\s"), " \\1 \\2 ")
            .replace(QRegularExpression("https?:\\/\\/([\\w\\-\\.]+)\\.\\w+\\/?[^\\s]*\\s"), " \\1 ");

    int length = 0;
    if (text.contains(QRegularExpression("[\\.!?]")))
    {
        length = 2;
        if (text.contains(QRegularExpression("^\\s*[A-ZА-ЯЁ]"))
                || text.contains(QRegularExpression("[\\.!?]\\s*[A-ZА-ЯЁ]")))
            ++wordCounts[".normal_case"];
        else
            ++wordCounts[".lower_case"];

        if (text.contains(QRegularExpression("[A-ZА-ЯЁa-zа-яё][\\.,!?]\\s+[A-ZА-ЯЁa-zа-яё]")))
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

    ++wordCounts[QString(".type_%1").arg(entry->type())];
    ++wordCounts[QString(".author_%1").arg(entry->author()->slug())];

    if (entry->author()->isFemale())
        ++wordCounts[".female"];
    else
        ++wordCounts[".male"];

    if (entry->author()->isDaylog())
        ++wordCounts[".daylog"];
    else
        ++wordCounts[".wholelog"];

    if (entry->author()->isFlow())
        ++wordCounts[".flow"];
    else
        ++wordCounts[".tlog"];

    if (entry->author()->isPremium())
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

    _entriesChanged[type][entry->entryId()] = true;
    _total[type] +=  _calcEntry(entry, _wordCounts[type]);
}



Trainer* Bayes::trainer()
{
    if (!_trainer)
        _trainer = new Trainer(this);

    return _trainer;
}
