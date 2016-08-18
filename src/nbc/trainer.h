#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>
#include <QAbstractListModel>
#include <QFutureSynchronizer>
#include <QFutureWatcher>
#include <QReadWriteLock>

#include "../models/calendarmodel.h"
#include "../models/usersmodelbayes.h"

class Bayes;
class Tlog;
class User;



class Trainer : public QObject
{
    Q_OBJECT

    friend class UsersModelBayes;

    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)

    Q_PROPERTY(int      currentTlog         READ currentTlog            NOTIFY currentTlogChanged)
    Q_PROPERTY(int      tlogsCount          READ tlogsCount             NOTIFY tlogsCountChanged)
    Q_PROPERTY(int      trainedEntriesCount READ trainedEntriesCount    NOTIFY trainedEntriesCountChanged)
    Q_PROPERTY(int      entriesCount        READ entriesCount           NOTIFY entriesCountChanged)
    Q_PROPERTY(QString  currentName         READ currentName            NOTIFY currentNameChanged)

public:
    enum Mode {
        WaterMode = UsersModel::WaterMode,
        FireMode = UsersModel::FireMode,
        UndefinedMode
    };

    Q_ENUMS(Mode)

    explicit Trainer(Bayes* parent = 0);
    ~Trainer();

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const { return _curMode; }

    Q_INVOKABLE int     currentTlog()           const;
    Q_INVOKABLE int     tlogsCount()            const;
    Q_INVOKABLE int     trainedEntriesCount()   const;
    Q_INVOKABLE int     entriesCount()          const;
    Q_INVOKABLE QString currentName()           const;

    Q_INVOKABLE Mode    typeOfTlog(int id)      const;

signals:
    void modeChanged();

    void trainStarted(bool full);
    void trainFinished();

    void currentTlogChanged();
    void tlogsCountChanged();

    void trainedEntriesCountChanged();
    void entriesCountChanged();

    void currentNameChanged();

public slots:
    void train();
    void trainTlog(const int tlogId, QString name, const Mode mode);

private slots:
    void _runAddingEntries();
    void _loadEntries();
    void _trainNextTlog();
    void _trainEntry();
    void _incTrainedCount();
    void _finishTraining();

private:
    struct BayesTlog {
        explicit BayesTlog(int userId = 0, int last = 0);
        explicit BayesTlog(User* user);
        ~BayesTlog();
        inline bool operator==(const BayesTlog& other);
        inline bool operator==(const int& userId);
        void loadInfo();
        User* user;
        int id;
        int latest;
    };

    int _findTlog(int& type, int id) const;

    void _initDb();
    void _loadDb();
    void _saveDb();

    void _addEntriesToLoad();

    Bayes* _bayes;

    Mode                _curMode;
    int                 _iCurTlog;
    CalendarModel*      _curTlog;
    QList<BayesTlog>    _tlogs[2];
    
    QList<CalendarEntry*> _entries;

    int _trainedEntriesCount;
    int _trainingEntriesCount;
    int _loadAfter;

    QString _curName;

    QFutureWatcher<void>      _addWatcher;
    QFutureSynchronizer<void> _sync;
    mutable QReadWriteLock    _lock;
};

#endif // TRAINER_H
