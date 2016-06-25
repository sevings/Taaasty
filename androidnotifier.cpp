#include "androidnotifier.h"

#include <QAndroidJniObject>

#include <QDebug>



AndroidNotifier::AndroidNotifier(QObject *parent) : QObject(parent)
{

}

void AndroidNotifier::setNotification(const QString& notification)
{
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(notification);
    QAndroidJniObject::callStaticMethod<void>("org/binque/taaasty/NotificationClient",
                                       "notify",
                                       "(Ljava/lang/String;)V",
                                       javaNotification.object<jstring>());
}
