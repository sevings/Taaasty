#ifndef BAYES_H
#define BAYES_H

#include <QObject>
#include <QHash>
#include <QSqlDatabase>
#include <QReadWriteLock>

class EntryBase;
class Trainer;
class StemmerV;

class QTimer;



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

signals:

public slots:

private slots:
    void _saveDb();

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

    Trainer* _trainer;
    StemmerV* _stemmer;
    QSqlDatabase _db;
    QTimer* _saveTimer;
    mutable QReadWriteLock _lock;
};

#endif // BAYES_H
