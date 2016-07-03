#ifndef CALENDARMODEL_H
#define CALENDARMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>

class CalendarEntry;
class Entry;



class CalendarModel : public QAbstractListModel
{
    Q_OBJECT

public:
    CalendarModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void setTlog(const int tlog);

    int lastEntryId() const;

    CalendarEntry* at(int row) const;
    
signals:
    void loaded();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _setCalendar(QJsonObject data);

private:
    QList<CalendarEntry*> _calendar;
};

#endif // CALENDARMODEL_H