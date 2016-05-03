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

    int loadedEntriesCount() const { return _loadedEntriesCount; }

public slots:
    void loadAllEntries();

signals:
    void entryLoaded(const Entry* entry);
    void loadedEntriesCountChanged();
    void allEntriesLoaded();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _setCalendar(QJsonObject data);
    void _emitEntryLoaded();

private:
    QList<CalendarEntry*> _calendar;
    int _loadedEntriesCount;
};

#endif // CALENDARMODEL_H
