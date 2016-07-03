#include "CalendarEntry.h"

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
    _createdAt       = Tasty::parseDate(data.value("created_at").toString());
    _url             = data.value("entry_url").toString();
    _type            = data.value("type").toString();
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



EntryBase*CalendarEntry::base()
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

