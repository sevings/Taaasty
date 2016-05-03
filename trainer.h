#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>
#include <QAbstractListModel>

class Bayes;
class Tlog;



class Trainer : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode)

public:
    enum Mode {
        WaterMode,
        FireMode
    };

    Q_ENUMS(Mode)

    explicit Trainer(Bayes* parent = 0);
    ~Trainer();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _curType; }

signals:

public slots:

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:

private:
    struct BayesTlog {
        BayesTlog(int tlogId = 0, int last = 0);
        void loadInfo();
        Tlog* tlog;
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

    Mode _curType;
    int  _iCurTlog;
    BayesTlog* _curTlog;

    int _lastFavorite;

    int _entriesLoaded;
    int _entriesLoadedTotal;
    int _entriesTotal;
};

#endif // TRAINER_H
