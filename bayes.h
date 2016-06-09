#ifndef BAYES_H
#define BAYES_H

#include <QObject>
#include <QMap>
#include <QSqlDatabase>

class Entry;
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

    int classify(const Entry* entry, const int minLength = 0) const;
    int voteForEntry(const Entry* entry, const Type type);
    int entryVoteType(const Entry* entry) const;

    Trainer* trainer();

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

    int _calcText(QString text, QMap<QString, FeatureCount>& wordCounts) const;
    int _calcEntry(const Entry* entry, QMap<QString, FeatureCount>& wordCounts, const int minLength = 0) const;
    int _addEntry(const Entry* entry, const Type type);

    QMap<QString, FeatureCount> _wordCounts[2];
    QMap<int, bool>             _entriesChanged[2];
    int                          _total[2];

    bool _loaded;

    Trainer* _trainer;
    StemmerV* _stemmer;
    QSqlDatabase _db;
    QTimer* _saveTimer;
};

#endif // BAYES_H
