#ifndef BAYES_H
#define BAYES_H

#include <QObject>
#include <QHash>

class Entry;



class Bayes : public QObject
{
    Q_OBJECT
public:
    explicit Bayes(QObject *parent = 0);
    ~Bayes();

    static Bayes* instance(QObject* parent = nullptr);

    int classify(const Entry* entry, const int minLength);

signals:

public slots:

private:
    enum Type { Water, Fire };

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
//    struct BayesEntry {
//        BayesEntry(qint16 entryId)
//            : id(entryId), changed(false) { }
//        int id;
//        bool changed;
//    };
    struct BayesTlog {
        BayesTlog(int tlogId = 0, int last = 0)
            : id (tlogId), latest(last), include(true), removed(false) { }
        int id;
        int latest;
        bool include;
        bool removed;
    };

    void _initDb();
    void _loadDb();
    void _saveDb();

    bool      _isEntryAdded(int id);
    BayesTlog _findTlog(int id, bool included = false);

    int _calcText(QString text, QHash<QString, FeatureCount>& wordCounts);
    int _calcEntry(const Entry* entry, QHash<QString, FeatureCount>& wordCounts, const int minLength = 0);
    int _addEntry(const Entry* entry, const Type type);

    bool _loaded;

    QHash<QString, FeatureCount> _wordCounts[2];
    QList<BayesTlog>             _tlogs[2];
    QHash<int, bool>             _entriesChanged[2];
    int                          _total[2];
    int                          _lastFavorite;
};

#endif // BAYES_H
