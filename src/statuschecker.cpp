#include "statuschecker.h"

#include "apirequest.h"
#include "data/Author.h"



StatusChecker::StatusChecker(QObject* parent)
    : QObject(parent)
{
    _timer.setInterval(300000);
    _timer.setSingleShot(false);
    _timer.start();

    Q_TEST(connect(&_timer, &QTimer::timeout, this, &StatusChecker::_checkStatuses));
}



void StatusChecker::add(Author* author)
{
    if (!author || _authors.values(author->id()).contains(author))
        return;

    _authors.insert(author->id(), author);
}



void StatusChecker::remove(Author* author)
{
    if (!author)
        return;

    _authors.remove(author->id(), author);
}



void StatusChecker::clear()
{
    _authors.clear();
    _timer.start();
}



void StatusChecker::_checkStatuses()
{
    QSet<int> ids;
    for (auto author: _authors)
        if (author)
            ids << author->id();

    if (ids.isEmpty())
        return;

    QString arg;
    arg.reserve(qMin(ids.size(), 100) * 8);
    int i = 0;

    auto check = [&]()
    {
        arg.remove(arg.size() - 1, 1);
        auto request = new ApiRequest(QStringLiteral("v1/online_statuses.json?user_ids=") + arg);
        request->get();

        Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_initStatuses(QJsonArray))));

        i = 0;
        arg.clear();
    };

    for (auto id: ids)
    {
        arg += QString::number(id) + ',';
        if (++i >= 100)
            check();
    }

    if (i > 0)
        check();
}



void StatusChecker::_initStatuses(const QJsonArray& data)
{
    for (auto statusVal: data)
    {
        auto status = statusVal.toObject();
        auto id = status.value(QLatin1String("id")).toInt();
        auto authors = _authors.values(id);
        for (auto author: authors)
        {
            if (author)
                author->_initStatus(status);
            else
                _authors.remove(id, author);
        }
    }
}
