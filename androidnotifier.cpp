#include "androidnotifier.h"

#ifdef Q_OS_ANDROID
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
#endif
