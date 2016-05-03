#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>

class Bayes;



class Trainer : public QObject
{
    Q_OBJECT
public:
    explicit Trainer(Bayes* parent = 0);
    ~Trainer();

signals:

public slots:

private:
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

    BayesTlog _findTlog(int id, bool included = false);

    Bayes* _bayes;

    QList<BayesTlog> _tlogs[2];
    int              _lastFavorite;
};

#endif // TRAINER_H
