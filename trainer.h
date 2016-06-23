#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>
#include <QAbstractListModel>

#include "calendarmodel.h"
#include "usersmodelbayes.h"

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
    void trainTlog(const int tlogId, const Mode mode);

private slots:
    void _trainNextTlog();
    void _trainEntry(const Entry* entry);
    void _finishTraining();

private:
    struct BayesTlog {
        BayesTlog(int userId = 0, int last = 0);
        BayesTlog(User* user);
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

    Bayes* _bayes;

    Mode                _curMode;
    int                 _iCurTlog;
    CalendarModel*      _curTlog;
    QList<BayesTlog>    _tlogs[2];
};

#endif // TRAINER_H
