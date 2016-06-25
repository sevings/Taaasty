#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

#ifdef Q_OS_ANDROID
#include <QObject>

class AndroidNotifier : public QObject
{
    Q_OBJECT
public:
    explicit AndroidNotifier(QObject *parent = 0);

    void setNotification(const QString &notification);

signals:

public slots:
};

#endif
#endif // ANDROIDNOTIFIER_H
