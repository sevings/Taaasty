#include "trainer.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

#include <QDebug>

#include "datastructures.h"
#include "bayes.h"



Trainer::Trainer(Bayes* parent)
    : QAbstractListModel(parent)
    , _bayes(parent)
    , _curType(WaterMode)
    , _iCurTlog(0)
    , _curTlog(nullptr)
    , _lastFavorite(0)
    , _entriesLoaded(0)
    , _entriesLoadedTotal(0)
    , _entriesTotal(0)
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



QHash<int, QByteArray> Trainer::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "user";
    return roles;
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
    Q_ASSERT(query.exec("CREATE TABLE IF NOT EXISTS bayes_tlogs   (type INTEGER, tlog INTEGER, latest INTEGER, n INTEGER, PRIMARY KEY(tlog))"));
}



void Trainer::_loadDb()
{
    _initDb();

    QSqlQuery query;
    Q_ASSERT(query.exec("SELECT type, tlog, latest, n FROM bayes_tlogs WHERE tlog > 0 ORDER BY n"));
    while (query.next())
        _tlogs[query.value(0).toInt()] << BayesTlog(query.value(1).toInt(),
                                                    query.value(2).toInt());

    Q_ASSERT(query.exec("SELECT latest FROM bayes_tlogs WHERE tlog = -1"));
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
                Q_ASSERT(query.prepare("INSERT OR REPLACE INTO bayes_tlogs VALUES (?, ?, ?, ?)"));
                query.addBindValue(type);
                query.addBindValue(_tlogs[tlog].at(tlog).id);
                query.addBindValue(_tlogs[tlog].at(tlog).latest);
                query.addBindValue(tlog);
                Q_ASSERT(query.exec());
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
