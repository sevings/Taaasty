#include "trainer.h"

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "defines.h"

#include "datastructures.h"
#include "bayes.h"



Trainer::Trainer(Bayes* parent)
    : QObject(parent)
    , _bayes(parent)
    , _curMode(WaterMode)
    , _iCurTlog(0)
{
    Q_ASSERT(_bayes);

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
}



int Trainer::currentTlog() const
{
    return _curMode == WaterMode ? _iCurTlog + 1 : _tlogs[WaterMode].size() + _iCurTlog + 1;
}



int Trainer::tlogsCount() const
{
    return _tlogs[WaterMode].size() + _tlogs[FireMode].size();
}



int Trainer::trainedEntriesCount() const
{
    return _curTlog ? _curTlog->loadedEntriesCount() : 0;
}



int Trainer::entriesCount() const
{
    return _curTlog ? _curTlog->loadingEntriesCount() : 0;
}



QString Trainer::currentName() const
{
    return _iCurTlog < _tlogs[_curMode].size()
            ? _tlogs[_curMode].at(_iCurTlog).user->name() : "";
}



void Trainer::train()
{
//    for (int type = 0; type < 2; type++)
//        for (int tlog = 0; tlog < _users->_tlogs[type].size(); tlog++)
//            if (!_users->_tlogs[type].at(tlog).include
//                    || _users->_tlogs[type].at(tlog).removed)
//                _users->_tlogs[type].removeAt(tlog);

    _curMode = WaterMode;
    _iCurTlog = -1;

    _trainNextTlog();

    emit trainStarted(true);
}



void Trainer::trainTlog(const int tlogId, const Trainer::Mode mode)
{
    if (tlogId <= 0 || mode != Trainer::WaterMode || mode != Trainer::FireMode)
        return;

    _curMode = mode;

    auto calendar = new CalendarModel(this);
    calendar->setTlog(tlogId);

    connect(calendar, SIGNAL(allEntriesLoaded()),    calendar, SLOT(deleteLater()));
    connect(calendar, SIGNAL(entryLoaded(const Entry*)), this, SLOT(_trainEntry(const Entry*)));

    calendar->loadAllEntries();
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
                emit trainFinished();
                return;
            }

            _curMode = FireMode;
            _iCurTlog = 0;
        }
        else
        {
            emit trainFinished();
            return;
        }
    }

    auto tlog = _tlogs[_curMode].at(_iCurTlog);
    auto calendar = new CalendarModel(this);
    calendar->setTlog(tlog.id);

    connect(calendar, SIGNAL(allEntriesLoaded()),    calendar, SLOT(deleteLater()));
    connect(calendar, SIGNAL(allEntriesLoaded()),        this, SLOT(_trainNextTlog()));
    connect(calendar, SIGNAL(entryLoaded(const Entry*)), this, SLOT(_trainEntry(const Entry*)));

    calendar->loadAllEntries(tlog.latest);
}



void Trainer::_trainEntry(const Entry* entry)
{
    _bayes->_addEntry(entry, (Bayes::Type)_curMode);
}



int Trainer::_findTlog(int& type, int id)
{
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
