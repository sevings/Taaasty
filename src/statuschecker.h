#ifndef STATUSCHECKER_H
#define STATUSCHECKER_H

#include <QObject>
#include <QPointer>
#include <QHash>
#include <QJsonArray>

#include "defines.h"

class Author;



class StatusChecker : public QObject
{
    Q_OBJECT
public:
    explicit StatusChecker(QObject* parent = nullptr);

    void addAuthor(Author* author);

private slots:
    void _checkStatuses();
    void _initStatuses(const QJsonArray& data);

private:
    QList<QPointer<Author>> _unchecked;
    QMultiHash<int, QPointer<Author>> _authors;
};

#endif // STATUSCHECKER_H
