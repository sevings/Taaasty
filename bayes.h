#ifndef BAYES_H
#define BAYES_H

#include <QObject>
#include <QHash>



class Bayes : public QObject
{
    Q_OBJECT
public:
    explicit Bayes(QObject *parent = 0);

    static Bayes* instance();

signals:

public slots:

private:
    enum Type { Water, Fire };

    struct FeatureCount {
        FeatureCount(quint16 c = 0)
            : count(c), changed(false) { }
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
        BayesTlog(int tlogId, int last)
            : id (tlogId), latest(last), include(true), removed(false) { }
        int id;
        int latest;
        bool include;
        bool removed;
    };

    void _initDb();
    void _loadDb();
    void _saveDb();

    bool _loaded;

    QHash<QString, FeatureCount> _wordCounts[2];
    QList<BayesTlog>             _tlogs[2];
    QHash<int, bool>             _entriesChanged[2];
    int                          _total[2];
    int                          _lastFavorite;
};

#endif // BAYES_H
