#include "calendarmodel.h"

#include <QJsonArray>
#include <QDebug>

#include "../defines.h"

#include "../data/CalendarEntry.h"
#include "../apirequest.h"



CalendarModel::CalendarModel(QObject* parent)
    : QAbstractListModel(parent)
{
    qDebug() << "CalendarModel";
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

    endResetModel();
}



int CalendarModel::lastEntryId() const
{
    if (_calendar.isEmpty())
        return 0;

    return _calendar.last()->id(); // TODO: not zero
}



CalendarEntry* CalendarModel::at(int row) const
{
    Q_ASSERT(row > 0 && row < _calendar.size());
    
    return _calendar.at(row);
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

    emit loaded();
}
