#include "CalendarEntry.h"

#include "../defines.h"

#include "Entry.h"
#include "../tasty.h"



CalendarEntry::CalendarEntry(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _entry(nullptr)
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



int CalendarEntry::id() const
{
    return _id;
}

