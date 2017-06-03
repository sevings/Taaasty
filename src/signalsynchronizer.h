#ifndef SIGNALSYNCHRONIZER_H
#define SIGNALSYNCHRONIZER_H

#include <QObject>
#include <QSet>

#include "defines.h"



class SignalSynchronizer : public QObject
{
    Q_OBJECT

public:
    explicit SignalSynchronizer(QObject* parent = nullptr);

    void add(const QObject* obj);
    void watch();

signals:
    void finished();

public slots:
    void sync();

private:
    QSet<const QObject*> _objects;

    bool _watching;
};

#endif // SIGNALSYNCHRONIZER_H
