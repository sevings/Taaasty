#ifndef BAYES_H
#define BAYES_H

#include <QObject>
#include <QHash>

class Entry;



class Bayes : public QObject
{
    Q_OBJECT

    friend class Trainer;

public:
    enum Type { Water, Fire };

    explicit Bayes(QObject *parent = 0);
    ~Bayes();

    static Bayes* instance(QObject* parent = nullptr);

    int classify(const Entry* entry, const int minLength = 0);
    int voteForEntry(const Entry* entry, const Type type);

signals:

public slots:

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
//    struct BayesEntry {
//        BayesEntry(qint16 entryId)
//            : id(entryId), changed(false) { }
//        int id;
//        bool changed;
//    };

    void _initDb();
    void _loadDb();
    void _saveDb();

    bool _isEntryAdded(int id);

    int _calcText(QString text, QHash<QString, FeatureCount>& wordCounts);
    int _calcEntry(const Entry* entry, QHash<QString, FeatureCount>& wordCounts, const int minLength = 0);
    int _addEntry(const Entry* entry, const Type type);

    bool _loaded; // todo: remove if not needed

    QHash<QString, FeatureCount> _wordCounts[2];
    QHash<int, bool>             _entriesChanged[2];
    int                          _total[2];
};

#endif // BAYES_H
