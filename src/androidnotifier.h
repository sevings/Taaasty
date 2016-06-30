#ifndef ANDROIDNOTIFIER_H
#define ANDROIDNOTIFIER_H

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

#endif // ANDROIDNOTIFIER_H
