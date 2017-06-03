#include "signalsynchronizer.h"



SignalSynchronizer::SignalSynchronizer(QObject* parent)
    : QObject(parent)
    , _watching(false)
{

}



void SignalSynchronizer::add(const QObject* obj)
{
    _objects << obj;
}



void SignalSynchronizer::watch()
{
    _watching = !_objects.isEmpty();
    if (!_watching)
        Q_TEST(QMetaObject::invokeMethod(this, "finished", Qt::QueuedConnection));
}



void SignalSynchronizer::clear()
{
    _objects.clear();
    _watching = false;
}



void SignalSynchronizer::sync()
{
    auto obj = sender();
    if (!obj)
        return;

    _objects.remove(obj);
    if (!_objects.isEmpty())
        return;

    _watching = false;
    emit finished();
}
