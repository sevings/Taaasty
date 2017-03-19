#include "statuschecker.h"

#include "apirequest.h"
#include "data/Author.h"



StatusChecker::StatusChecker(QObject* parent)
    : QObject(parent)
{

}



void StatusChecker::addAuthor(Author* author)
{
    if (!author)
        return;

    _unchecked << author;

    Q_TEST(QMetaObject::invokeMethod(this, "_checkStatuses", Qt::QueuedConnection));
}



void StatusChecker::_checkStatuses()
{
    QSet<int> ids;
    for (auto author: _unchecked)
    {
        ids << author->id();
        _authors.insert(author->id(), author);
    }

    _unchecked.clear();

    if (ids.isEmpty())
        for (auto author: _authors)
            ids << author->id();

    if (ids.isEmpty())
        return;
}



void StatusChecker::_initStatuses(const QJsonArray& data)
{

}
