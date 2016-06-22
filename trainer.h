#ifndef TRAINER_H
#define TRAINER_H

#include <QObject>
#include <QAbstractListModel>

#include "calendarmodel.h"
#include "usersmodelbayes.h"

class Bayes;
class Tlog;



class Trainer : public QObject
{
    Q_OBJECT

public:
    enum Mode {
        WaterMode = UsersModel::WaterMode,
        FireMode = UsersModel::FireMode
    };

    explicit Trainer(Bayes* parent = 0);
    ~Trainer();

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const { return _curMode; }

    Q_INVOKABLE int     currentTlog()           const;
    Q_INVOKABLE int     tlogsCount()            const;
    Q_INVOKABLE int     trainedEntriesCount()   const;
    Q_INVOKABLE int     entriesCount()          const;
    Q_INVOKABLE QString currentName()           const;

signals:
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

private:
    Bayes* _bayes;

    Mode                _curMode;
    int                 _iCurTlog;
    CalendarModel*      _curTlog;
    UsersModelBayes*    _users;
};

#endif // TRAINER_H
