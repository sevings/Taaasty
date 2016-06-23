#include "calendarmodel.h"

#include <QJsonArray>

#include "defines.h"

#include "datastructures.h"
#include "apirequest.h"



CalendarModel::CalendarModel(QObject* parent)
    : QAbstractListModel(parent)
    , _loadedEntriesCount(0)
    , _loadingEntriesCount(0)
    , _loadAfter(0)
{

}



int CalendarModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return _calendar.size();
}



QVariant CalendarModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _calendar.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<CalendarEntry*>(_calendar.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



void CalendarModel::setTlog(const int tlog)
{
    if (tlog <= 0)
        return;

    QString url = QString("tlog/%1/calendar.json").arg(tlog);
    auto request = new ApiRequest(url);
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_setCalendar(QJsonObject))));

    beginResetModel();

    qDeleteAll(_calendar);
    _calendar.clear();

    _loadedEntriesCount = 0;
    _loadingEntriesCount = 0;

    endResetModel();
}



void CalendarModel::loadAllEntries(const int after)
{
    if (_calendar.isEmpty())
    {
        _loadAfter = after;
        return;
    }

    _loadedEntriesCount = 0;
    _loadingEntriesCount = 0;

    foreach (auto entry, _calendar)
    {
        if (entry->_id == after)
            break;

        _loadingEntriesCount++;

        auto full = entry->full();
        Q_TEST(connect(full, SIGNAL(updated()), this, SLOT(_emitEntryLoaded())));
    }

    if (_loadingEntriesCount == 0)
        emit allEntriesLoaded();

    emit loadingEntriesCountChanged();

    _loadAfter = 0;
}



QHash<int, QByteArray> CalendarModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "entry";
    return roles;
}



void CalendarModel::_setCalendar(QJsonObject data)
{
    beginResetModel();

    auto periods = data.value("periods").toArray();
    for (int i = 0; i < periods.size(); i++)
    {
        auto markers = periods.at(i).toObject().value("markers").toArray();
        for (int j = markers.size(); j >= 0; j--)
        {
            auto entry = markers.at(j).toObject();
            _calendar << new CalendarEntry(entry, this);
        }
    }

    endResetModel();

    if (_loadAfter)
            loadAllEntries(_loadAfter);
}



void CalendarModel::_emitEntryLoaded()
{
    auto entry = static_cast<Entry*>(sender());
    if (!entry)
        return;

    emit entryLoaded(entry);

    _loadedEntriesCount++;
    emit loadedEntriesCountChanged();

    if (_loadedEntriesCount >= _loadingEntriesCount)
        emit allEntriesLoaded();
}
