#include "androidimagepicker.h"

#include <QtAndroid>
#include <QDebug>

#include "../tasty.h"



AndroidImagePicker::AndroidImagePicker(QObject* parent)
    : _parent(parent)
{

}



void AndroidImagePicker::select() {
    auto actionPick = QAndroidJniObject::fromString(QStringLiteral("android.intent.action.GET_CONTENT"));
    QAndroidJniObject intent("android/content/Intent");
    if (!actionPick.isValid() || !intent.isValid())
        return;

    intent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;", actionPick.object<jstring>());
    intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                            QAndroidJniObject::fromString(QStringLiteral("image/*")).object<jstring>());
    QtAndroid::startActivity(intent.object<jobject>(), 101, this);
}



void AndroidImagePicker::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject& data)
{
    auto ok = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
    if (receiverRequestCode != 101 || resultCode != ok)
        return;

    auto imagePath = QAndroidJniObject::callStaticObjectMethod("org/binque/taaasty/RealPathUtil", "getPath",
                                                               "(Landroid/content/Context;Landroid/content/Intent;)Ljava/lang/String;",
                                                               QtAndroid::androidActivity().object(),
                                                               data.object()).toString();

    QAndroidJniEnvironment env;
    if (_catchException(env)) return;

    Q_TEST(QMetaObject::invokeMethod(_parent, "_append", Qt::QueuedConnection, Q_ARG(QString, imagePath)));
}



bool AndroidImagePicker::_catchException(QAndroidJniEnvironment& env) const
{
    if (!env->ExceptionCheck())
        return false;

    auto exception = env->ExceptionOccurred();
    env->ExceptionClear();

    auto exccls = env->GetObjectClass(exception);
    auto toString = env->GetMethodID(exccls, "toString", "()Ljava/lang/String;");
    auto message = static_cast<jstring>(env->CallObjectMethod(exception, toString));
    auto utfMessage = env->GetStringUTFChars(message, 0);

    qDebug() << "Java exception catched: " << utfMessage;
    emit pTasty->error(0, utfMessage);

    env->ReleaseStringUTFChars(message, utfMessage);
    return true;
}
