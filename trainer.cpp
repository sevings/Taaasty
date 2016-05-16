#include "trainer.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

#include "defines.h"

#include "datastructures.h"
#include "bayes.h"



Trainer::Trainer(Bayes* parent)
    : QAbstractListModel(parent)
    , _bayes(parent)
    , _curMode(WaterMode)
    , _iCurTlog(0)
    , _lastFavorite(0)
//    , _entriesLoaded(0)
//    , _entriesLoadedTotal(0)
//    , _entriesTotal(0)
{
    _loadDb();
}



Trainer::~Trainer()
{
    _saveDb();
}



int Trainer::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    return _tlogs[_curMode].size();
}



QVariant Trainer::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _tlogs[_curMode].size() || role != Qt::UserRole)
        return QVariant();

    auto tlog = _tlogs[_curMode].at(index.row());
    if (tlog.tlog->tlogId() == 0)
        tlog.loadInfo();

    return QVariant::fromValue<User*>(tlog.tlog->author());
}



void Trainer::setMode(const Trainer::Mode mode)
{
    if (mode == _curMode)
        return;

    beginResetModel();

    _curMode = mode;

    endResetModel();
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
            ? _tlogs[_curMode].at(_iCurTlog).tlog->_author->_name : "";
}



void Trainer::train()
{
    for (int type = 0; type < 2; type++)
        for (int tlog = 0; tlog < _tlogs[type].size(); tlog++)
            if (!_tlogs[type].at(tlog).include
                    || _tlogs[type].at(tlog).removed)
            {
                if (_curMode == type)
                    beginRemoveRows(QModelIndex(), tlog, tlog);

                _tlogs[type].removeAt(tlog);

                if (_curMode == type)
                    endRemoveRows();
            }

    _curMode = WaterMode;
    _iCurTlog = -1;

    _trainNextTlog();

    emit trainStarted(true);

//    _entriesLoaded      = 0;
//    _entriesTotal       = 0;
    //    _entriesLoadedTotal = 0;
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



QHash<int, QByteArray> Trainer::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "user";
    return roles;
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



Trainer::BayesTlog::BayesTlog(int tlogId, int last)
    : tlog(new Tlog)
    , id (tlogId)
    , latest(last)
    , include(true)
    , removed(false)
{

}



void Trainer::BayesTlog::loadInfo()
{
    tlog->setId(id);
}



void Trainer::_initDb()
{
    QSqlQuery query;
    Q_TEST(query.exec("CREATE TABLE IF NOT EXISTS bayes_tlogs   (type INTEGER, tlog INTEGER, latest INTEGER, n INTEGER, PRIMARY KEY(tlog))"));
}



void Trainer::_loadDb()
{
    _initDb();

    QSqlQuery query;
    Q_TEST(query.exec("SELECT type, tlog, latest, n FROM bayes_tlogs WHERE tlog > 0 ORDER BY n"));
    while (query.next())
        _tlogs[query.value(0).toInt()] << BayesTlog(query.value(1).toInt(),
                                                    query.value(2).toInt());

    Q_TEST(query.exec("SELECT latest FROM bayes_tlogs WHERE tlog = -1"));
    if (query.next())
        _lastFavorite = query.value(0).toInt();
}



void Trainer::_saveDb()
{
    QSqlQuery query;
    for (int type = 0; type < 2; type++)
        for (int tlog = 0; tlog < _tlogs[type].size(); tlog++)
            if (_tlogs[type].at(tlog).include && !_tlogs[type].at(tlog).removed)
            {
                Q_TEST(query.prepare("INSERT OR REPLACE INTO bayes_tlogs VALUES (?, ?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(_tlogs[tlog].at(tlog).id);
                query.addBindValue(_tlogs[tlog].at(tlog).latest);
                query.addBindValue(tlog);
                Q_TEST(query.exec());
            }
}



Trainer::BayesTlog Trainer::_findTlog(int id, bool included)
{
    for (int type = 0; type < 2; type++)
        foreach (auto tlog, _tlogs[type])
            if (tlog.id == id)
            {
                if (!tlog.removed
                        && ((included && tlog.include) || !included))
                    return tlog;
                else
                    return BayesTlog();
            }

    return BayesTlog();
}
