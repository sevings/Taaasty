#include "CalendarEntry.h"

#include <QDate>

#include "../defines.h"

#include "Entry.h"
#include "../tasty.h"



CalendarEntry::CalendarEntry(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _isPrivate(false)
    , _commentsCount(0)
    , _isFlow(false)
    , _entry(nullptr)
    , _base(nullptr)
{

}



CalendarEntry::CalendarEntry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entry(nullptr)
    , _base(nullptr)
{
    _id              = data.value("entry_id").toInt();
    auto d = data.value("created_at").toString();
    _createdAt       = Tasty::parseDate(d);

    auto date = QDate::fromString(d.left(7), "yyyy-MM");
    auto month = QDate::longMonthName(date.month(), QDate::StandaloneFormat);
    _month           = QString("%1, %2").arg(date.year()).arg(month);

    _url             = data.value("entry_url").toString();
    _type            = data.value("type_symbol").toString();
    _isPrivate       = data.value("is_private").toBool();
    _commentsCount   = data.value("comments_count").toInt();
    _truncatedTitle  = data.value("title_truncated").toString();
    _isFlow          = data.value("is_flow").toBool();
}



Entry* CalendarEntry::full()
{
    if (_entry)
        return _entry;

    _entry = new Entry(QJsonObject(), this);
    _entry->setId(_id);
    return _entry;
}



EntryBase* CalendarEntry::base()
{
    if (_base)
        return _base;

    _base = new EntryBase(this);
    _base->load(_id);
    return _base;
}



int CalendarEntry::id() const
{
    return _id;
}



QString CalendarEntry::month() const
{
    return _month;
}

