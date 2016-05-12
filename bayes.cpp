#include "bayes.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QRegularExpression>
#include <QtMath>

#ifdef QT_DEBUG
#   include <QDateTime>
#endif

#include "defines.h"

#include "datastructures.h"
#include "trainer.h"



Bayes::Bayes(QObject *parent)
    : QObject(parent)
    , _loaded(false)
{
    _total[Water] = 0;
    _total[Fire]  = 0;

    _loadDb();

    new Trainer(this);
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



int Bayes::classify(const Entry *entry, const int minLength)
{
    if (!_total[Water] || !_total[Fire])
        return 0;

    float values[2];
    QHash<QString, Bayes::FeatureCount> features;
    int length = _calcEntry(entry, features, minLength);
    if (length < 0)
        return length;

    for (int type = 0; type < 2; type++)
    {
        auto k = (_total[Water] + _total[Fire]) / (float) _total[type];
        values[type] = 0;
        foreach (auto word, features.keys())
        {
            int cnt = _wordCounts[type][word].count;
            if (cnt <= 0)
                continue;

            values[type] += qLn(cnt * k);
        }
    }

    auto result = (values[Fire] - values[Water]) / length * 50;
    return qRound(result);
}



int Bayes::voteForEntry(const Entry *entry, const Bayes::Type type)
{
    _total[type] += _addEntry(entry, type);
    return classify(entry);
}



void Bayes::_initDb()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("bayes");
    Q_TEST(db.open());

    QSqlQuery query;
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes         (type INTEGER, word TEXT, total INTEGER, PRIMARY KEY(type, word))"));
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes_entries (type INTEGER, entry INTEGER, PRIMARY KEY(entry))"));
}



void Bayes::_loadDb()
{
    if (_loaded)
        return;

    _initDb();

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

    _loaded = true;
}



void Bayes::_saveDb()
{
    if (!_loaded)
        return;

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    QSqlQuery query;
    for (int type = 0; type < 2; type++)
    {
        foreach (auto word, _wordCounts[type].keys())
            if (_wordCounts[type][word].changed)
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes VALUES (?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(word);
                query.addBindValue(_wordCounts[type][word].count);
                Q_TEST(query.exec());
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

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Saved in" << ms << "ms";
#endif
}



bool Bayes::_isEntryAdded(int id)
{
    return _entriesChanged[Water].contains(id)
            || _entriesChanged[Fire].contains(id);
}



int Bayes::_calcText(QString text, QHash<QString, Bayes::FeatureCount>& wordCounts)
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

    auto words = text.split(QRegularExpression("[^a-zA-Zа-яА-ЯёЁ]+"), QString::SkipEmptyParts);
    foreach (auto word, words)
    {
//        word = Stemmer.stem(word);
        ++wordCounts[word];
    }

    length += words.size();
    return length;
}



int Bayes::_calcEntry(const Entry *entry, QHash<QString, Bayes::FeatureCount> &wordCounts, const int minLength)
{
    int content = _calcText(entry->_text, wordCounts);
    int title   = _calcText(entry->_title, wordCounts);

    if (minLength > 0 && content < minLength && title < minLength)
        return -1;

    if (content > 100 || title > 100)
        ++wordCounts[".long"];
    else
        ++wordCounts[".short"];

    ++wordCounts[QString(".type_%1").arg(entry->_type)];
    ++wordCounts[QString(".author_%1").arg(entry->_author->_slug)];

    if (entry->_author->_isFemale)
        ++wordCounts[".female"];
    else
        ++wordCounts[".male"];

    if (entry->_author->_isDaylog)
        ++wordCounts[".daylog"];
    else
        ++wordCounts[".wholelog"];

    if (entry->_author->_isFlow)
        ++wordCounts[".flow"];
    else
        ++wordCounts[".tlog"];

    if (entry->_author->_isPremium)
        ++wordCounts[".premium"];
    else
        ++wordCounts[".free"];

    return content + title + 7;
}



int Bayes::_addEntry(const Entry *entry, const Bayes::Type type)
{
    if (_isEntryAdded(entry->_id))
        return 0;

    _entriesChanged[type].insert(entry->_id, false);
    return _calcEntry(entry, _wordCounts[type]);
}
