#include "trainer.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtConcurrent>

#include "../defines.h"

#include "../data/Tlog.h"
#include "../data/Entry.h"
#include "../data/CalendarEntry.h"
#include "../data/User.h"
#include "bayes.h"



Trainer::Trainer(Bayes* parent)
    : QObject(parent)
    , _bayes(parent)
    , _curMode(UndefinedMode)
    , _iCurTlog(0)
    , _curTlog(nullptr)
    , _trainedEntriesCount(0)
    , _trainingEntriesCount(0)
    , _loadAfter(0)
{
    Q_ASSERT(_bayes);

    qDebug() << "Trainer";

    Q_TEST(connect(&_addWatcher, SIGNAL(finished()), this, SLOT(_loadEntries())));

    _loadDb();
}



Trainer::~Trainer()
{
//    _saveDb();
}



void Trainer::setMode(const Trainer::Mode mode)
{
    if (mode == _curMode)
        return;

    _curMode = mode;
    emit modeChanged();
}



int Trainer::currentTlog() const
{
    QReadLocker lock(&_lock);

    switch(_curMode)
    {
    case WaterMode:
        return _iCurTlog + 1;
    case FireMode:
        return _tlogs[WaterMode].size() + _iCurTlog + 1;
    default:
        return 0;
    }
}



int Trainer::tlogsCount() const
{
    QReadLocker lock(&_lock);

    return _tlogs[WaterMode].size() + _tlogs[FireMode].size();
}



int Trainer::trainedEntriesCount() const
{
    QReadLocker lock(&_lock);

    return _trainedEntriesCount;
}



int Trainer::entriesCount() const
{
    QReadLocker lock(&_lock);

    return _trainingEntriesCount;
}



QString Trainer::currentName() const
{
    QReadLocker lock(&_lock);

    return _curMode != UndefinedMode && _iCurTlog < _tlogs[_curMode].size()
            ? _tlogs[_curMode].at(_iCurTlog).user->name() : "";
}



Trainer::Mode Trainer::typeOfTlog(int id) const
{
    QReadLocker lock(&_lock);

    int type = -1;
    auto i = _findTlog(type, id);
    if (i < 0)
        return UndefinedMode;

    return (Trainer::Mode)type;
}



void Trainer::train()
{
//    for (int type = 0; type < 2; type++)
//        for (int tlog = 0; tlog < _users->_tlogs[type].size(); tlog++)
//            if (!_users->_tlogs[type].at(tlog).include
//                    || _users->_tlogs[type].at(tlog).removed)
//                _users->_tlogs[type].removeAt(tlog);

    _curMode = WaterMode;
    emit modeChanged();

    _iCurTlog = -1;

    _trainNextTlog();

    emit trainStarted(true);
}



void Trainer::trainTlog(const int tlogId, const Trainer::Mode mode)
{
    if (tlogId <= 0 || (mode != Trainer::WaterMode && mode != Trainer::FireMode))
        return;

    _curMode = mode;
    emit modeChanged();

    _trainedEntriesCount  = 0;
    emit trainedEntriesCountChanged();

    _trainingEntriesCount = 0;
    emit entriesCountChanged();

    _curTlog = new CalendarModel(this);
    Q_TEST(connect(_curTlog, SIGNAL(loaded()), this, SLOT(_runAddingEntries())));
    _curTlog->setTlog(tlogId);

    int type;
    auto i = _findTlog(type, tlogId);
    if (i > 0)
    {
        _loadAfter = _tlogs[type].at(i).latest;
        _iCurTlog  = i;
    }
    else
    {
        _tlogs[mode] << BayesTlog(tlogId);
        _loadAfter = 0;
        _iCurTlog  = _tlogs[mode].size() - 1;
    }

    emit trainStarted(false);
}



void Trainer::_runAddingEntries()
{
    auto future = QtConcurrent::run(this, &Trainer::_addEntriesToLoad);
    _addWatcher.setFuture(future);
}



void Trainer::_loadEntries()
{
    foreach (auto entry, _entries)
    {
        auto base = entry->base();
        Q_TEST(connect(base, SIGNAL(loaded()),       this, SLOT(_trainEntry())));
        Q_TEST(connect(base, SIGNAL(loadingError()), this, SLOT(_incTrainedCount())));
    }
}



void Trainer::_trainNextTlog()
{
    _iCurTlog++;
    if (_iCurTlog >= _tlogs[_curMode].size())
    {
        if (_curMode == WaterMode)
        {
            if (_tlogs[FireMode].isEmpty())
            {
                _finishTraining();
                return;
            }

            _curMode = FireMode;
            emit modeChanged();

            _iCurTlog = 0;
        }
        else
        {
            _finishTraining();
            return;
        }
    }

    auto tlog = _tlogs[_curMode].at(_iCurTlog);
    _curTlog = new CalendarModel(this);
    _curTlog->setTlog(tlog.id);

//    Q_TEST(connect(_curTlog, SIGNAL(loadingEntriesCountChanged()), this, SIGNAL(entriesCountChanged())));
//    Q_TEST(connect(_curTlog, SIGNAL(loadedEntriesCountChanged()),  this, SIGNAL(trainedEntriesCountChanged())));

//    Q_TEST(connect(_curTlog, SIGNAL(allEntriesLoaded()),    _curTlog, SLOT(deleteLater())));
//    Q_TEST(connect(_curTlog, SIGNAL(allEntriesLoaded()),        this, SLOT(_trainNextTlog())));
//    Q_TEST(connect(_curTlog, SIGNAL(entryLoaded(const Entry*)), this, SLOT(_trainEntry(const Entry*))));
}



void Trainer::_trainEntry()
{
    auto entry = static_cast<EntryBase*>(sender());
    if (!entry)
        return;

    auto future = QtConcurrent::run(_bayes, &Bayes::_addEntry, entry, (Bayes::Type)_curMode);
    _sync.addFuture(future);

    _incTrainedCount();
}



void Trainer::_incTrainedCount()
{
    _trainedEntriesCount++;
    emit trainedEntriesCountChanged();

    if (_trainedEntriesCount >= _trainingEntriesCount)
        _finishTraining();
}



void Trainer::_finishTraining()
{
    _sync.waitForFinished();

    _tlogs[_curMode][_iCurTlog].latest = _curTlog->lastEntryId();

    _curMode = UndefinedMode;
    emit modeChanged();

    _entries.clear();
    _iCurTlog = 0;
    _curTlog->deleteLater();
    _curTlog = nullptr;

    _bayes->_saveDb();

    auto future = QtConcurrent::run(this, &Trainer::_saveDb);
    _sync.setFuture(future);

    emit trainFinished();
}



int Trainer::_findTlog(int& type, int id) const
{
    QReadLocker lock(&_lock);

    for (type = 0; type < 2; type++)
        for (int i = 0; i < _tlogs[type].size(); i++)
            if (_tlogs[type].at(i).id == id)
                    return i;

    return -1;
}



void Trainer::_initDb()
{
    QSqlQuery query;
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes_tlogs   (type INTEGER, tlog INTEGER, latest INTEGER, n INTEGER, PRIMARY KEY(tlog))"));
}



void Trainer::_loadDb()
{
    _initDb();

    QWriteLocker locker(&_lock);

    Q_TEST(_bayes->db().transaction());

    QSqlQuery query;
    Q_TEST(query.exec("SELECT type, tlog, latest, n FROM bayes_tlogs WHERE tlog > 0 ORDER BY n"));
    while (query.next())
        _tlogs[query.value(0).toInt()] << BayesTlog(query.value(1).toInt(),
                                                    query.value(2).toInt());

//    Q_TEST(query.exec("SELECT latest FROM bayes_tlogs WHERE tlog = -1"));
//    if (query.next())
//        _lastFavorite = query.value(0).toInt();

    Q_TEST(_bayes->db().commit());
}



void Trainer::_saveDb()
{
    QReadLocker lock(&_lock);

    Q_TEST(_bayes->db().transaction());

    QSqlQuery query;
    for (int type = 0; type < 2; type++)
    {
        if (_tlogs[type].isEmpty())
            continue; // TODO: but what if they all were deleted?

        Q_TEST(query.prepare("DELETE FROM bayes_tlogs WHERE type = ?"));
        query.addBindValue(type);
        Q_TEST(query.exec());

        for (int tlog = 0; tlog < _tlogs[type].size(); tlog++)
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes_tlogs VALUES (?, ?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(_tlogs[type].at(tlog).id);
                query.addBindValue(_tlogs[type].at(tlog).latest);
                query.addBindValue(tlog);
                Q_TEST(query.exec());
            }
    }

    Q_TEST(_bayes->db().commit());

    _bayes->_saveDb();
}



void Trainer::_addEntriesToLoad()
{
    for (int i = _curTlog->rowCount() - 1; i >= 0; i--)
    {
        auto entry = _curTlog->at(i);
        auto id = entry->id();

        if (id <= _loadAfter && _loadAfter > 0)
            break;

        if (id <= 0 || _bayes->_isEntryAdded(id))
            continue;

        _trainingEntriesCount++;

        _entries << entry;
    }

    if (_trainingEntriesCount == 0)
        _finishTraining();

    emit entriesCountChanged();

    _loadAfter = 0;
}



Trainer::BayesTlog::BayesTlog(int userId, int last)
    : user(new User)
    , id(userId)
    , latest(last)
{

}



Trainer::BayesTlog::BayesTlog(User* user)
    : user(user)
    , id(user->id())
    , latest(0)
{

}



Trainer::BayesTlog::~BayesTlog()
{
//    if (!user->parent()) //TODO: delete user
    //        delete user;
}



bool Trainer::BayesTlog::operator==(const Trainer::BayesTlog& other)
{
    return user == other.user
            || (user && other.user && user->id() == other.user->id());
}



bool Trainer::BayesTlog::operator==(const int& userId)
{
    return user && user->id() == userId;
}



void Trainer::BayesTlog::loadInfo()
{
    user->setId(id);
}
