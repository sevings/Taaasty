#ifndef DEFINES_H
#define DEFINES_H

#include <QDebug>

#ifdef QT_DEBUG
#   define Q_TEST(x) Q_ASSERT(x)
#else
#   define Q_TEST(x) x
#endif

#endif // DEFINES_H
