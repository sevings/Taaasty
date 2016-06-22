#include "trainer.h"

#include <QDebug>

#include "datastructures.h"
#include "bayes.h"



Trainer::Trainer(Bayes* parent)
    : QObject(parent)
    , _bayes(parent)
    , _curMode(WaterMode)
    , _iCurTlog(0)
    , _users(new UsersModelBayes(this))
{
    Q_ASSERT(_bayes);
}



Trainer::~Trainer()
{
}



void Trainer::setMode(const Trainer::Mode mode)
{
    if (mode == _curMode)
        return;

    _curMode = mode;
}



int Trainer::currentTlog() const
{
    return _curMode == WaterMode ? _iCurTlog + 1 : _users->_tlogs[WaterMode].size() + _iCurTlog + 1;
}



int Trainer::tlogsCount() const
{
    return _users->_tlogs[WaterMode].size() + _users->_tlogs[FireMode].size();
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
    return _iCurTlog < _users->_tlogs[_curMode].size()
            ? _users->_tlogs[_curMode].at(_iCurTlog).user->name() : "";
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
    if (_iCurTlog >= _users->_tlogs[_curMode].size())
    {
        if (_curMode == WaterMode)
        {
            if (_users->_tlogs[FireMode].isEmpty())
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

    auto tlog = _users->_tlogs[_curMode].at(_iCurTlog);
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
