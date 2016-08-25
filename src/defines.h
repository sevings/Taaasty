#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>
#include <QSharedPointer>

#ifdef QT_DEBUG
#   define Q_TEST(x) Q_ASSERT(x)
#else
#   define Q_TEST(x) x
#endif

class Entry;
typedef QSharedPointer<Entry> EntryPtr;

class Conversation;
typedef QSharedPointer<Conversation> ChatPtr;

#endif // DEFINES_H
