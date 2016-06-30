#include "androidnotifier.h"

#ifdef Q_OS_ANDROID
#   include <QAndroidJniObject>
#endif

#include <QDebug>



AndroidNotifier::AndroidNotifier(QObject *parent) : QObject(parent)
{
    qDebug() << "AndroidNotifier";
}

void AndroidNotifier::setNotification(const QString& notification)
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject javaNotification = QAndroidJniObject::fromString(notification);
    QAndroidJniObject::callStaticMethod<void>("org/binque/taaasty/NotificationClient",
                                       "notify",
                                       "(Ljava/lang/String;)V",
                                       javaNotification.object<jstring>());
#else
    Q_UNUSED(notification);
#endif
}
