#include "calendarmodel.h"

#include <QJsonArray>

#include "datastructures.h"
#include "apirequest.h"



CalendarModel::CalendarModel(QObject* parent)
    : QAbstractListModel(parent)
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
    Q_ASSERT(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_setCalendar(QJsonObject))));
}



void CalendarModel::loadAllEntries()
{
    _loadedEntriesCount = 0;

    foreach (auto entry, _calendar)
    {
        auto full = entry->full();
        Q_ASSERT(connect(full, SIGNAL(updated()), this, SLOT(_emitEntryLoaded())));
    }
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

    qDeleteAll(_calendar);
    _calendar.clear();

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
}



void CalendarModel::_emitEntryLoaded()
{
    auto entry = static_cast<Entry*>(sender());
    if (!entry)
        return;

    emit entryLoaded(entry);

    _loadedEntriesCount++;
    emit loadedEntriesCountChanged();

    if (_loadedEntriesCount >= _calendar.size())
        emit allEntriesLoaded();
}
