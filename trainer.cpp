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
    , _curType(WaterMode)
    , _iCurTlog(0)
    , _curTlog(nullptr)
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

    return _tlogs[_curType].size();
}



QVariant Trainer::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _tlogs[_curType].size() || role != Qt::UserRole)
        return QVariant();

    auto tlog = _tlogs[_curType].at(index.row());
    if (tlog.tlog->tlogId() == 0)
        tlog.loadInfo();

    return QVariant::fromValue<User*>(tlog.tlog->author());
}



void Trainer::setMode(const Trainer::Mode mode)
{
    if (mode == _curType)
        return;

    beginResetModel();

    _curType = mode;

    endResetModel();
}



int Trainer::currentTlog() const
{
    return _curType == WaterMode ? _iCurTlog + 1 : _tlogs[WaterMode].size() + _iCurTlog + 1;
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
    return _iCurTlog < _tlogs[_curType].size()
            ? _tlogs[_curType].at(_iCurTlog).tlog->_author->_name : "";
}



void Trainer::train()
{
    for (int type = 0; type < 2; type++)
        for (int tlog = 0; tlog < _tlogs[type].size(); tlog++)
            if (!_tlogs[type].at(tlog).include
                    || _tlogs[type].at(tlog).removed)
            {
                if (_curType == type)
                    beginRemoveRows(QModelIndex(), tlog, tlog);

                _tlogs[type].removeAt(tlog);

                if (_curType == type)
                    endRemoveRows();
            }

    _curType = WaterMode;
    _iCurTlog = -1;

    _trainNextTlog();

    emit trainStarted(true);

//    _entriesLoaded      = 0;
//    _entriesTotal       = 0;
//    _entriesLoadedTotal = 0;
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
    if (_iCurTlog >= _tlogs[_curType].size() && _curType == WaterMode)
    {
        if (_tlogs[FireMode].isEmpty())
            return; // TODO: finish

        _curType = FireMode;
        _iCurTlog = 0;
    }

    auto tlog = _tlogs[_curType].at(_iCurTlog);
    auto calendar = new CalendarModel(this);
    calendar->setTlog(tlog.id);

    connect(calendar, SIGNAL(allEntriesLoaded()), calendar, SLOT(deleteLater()));
    connect(calendar, SIGNAL(allEntriesLoaded()), this, SLOT(_trainNextTlog()));

    calendar->loadAllEntries(tlog.latest); // wait for calendar loading
}



void Trainer::_trainEntry()
{

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
            if (tlog.id == id && !tlog.removed
                    && ((included && tlog.include) || !included))
                return tlog;

    return BayesTlog();
}
