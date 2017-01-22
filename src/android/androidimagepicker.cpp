#include "androidimagepicker.h"

#include <QtAndroid>
#include <QDebug>

#include "../tasty.h"



AndroidImagePicker::AndroidImagePicker(QObject* parent)
    : _parent(parent)
{

}



void AndroidImagePicker::select() {
    auto actionPick = QAndroidJniObject::fromString("android.intent.action.GET_CONTENT");
    QAndroidJniObject intent("android/content/Intent");
    if (!actionPick.isValid() || !intent.isValid())
        return;

    intent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;", actionPick.object<jstring>());
    intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;", QAndroidJniObject::fromString("image/*").object<jstring>());
    QtAndroid::startActivity(intent.object<jobject>(), 101, this);
}


// http://stackoverflow.com/a/34295029
void AndroidImagePicker::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject& data)
{
    auto ok = QAndroidJniObject::getStaticField<jint>("android/app/Activity", "RESULT_OK");
    if (receiverRequestCode != 101 || resultCode != ok)
        return;

    QAndroidJniEnvironment env;
    
    // http://stackoverflow.com/a/30681610
    auto pattern = QAndroidJniObject::callStaticObjectMethod("java/util/regex/Pattern", "compile",
                                                             "(Ljava/lang/String;)Ljava/util/regex/Pattern;",
                                                             env->NewStringUTF("(content://media/.*\\d)"));
    if (_catchException(env)) return;

    auto uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
    auto uriPath = uri.callObjectMethod("getPath", "()Ljava/lang/String;");
    qDebug() << "image uri path:" << uriPath.toString();

    auto uriPathStringUTF = env->NewStringUTF(uriPath.toString().toStdString().c_str());
    auto matcher = pattern.callObjectMethod("matcher", "(Ljava/lang/CharSequence;)Ljava/util/regex/Matcher;", uriPathStringUTF);
    auto found = matcher.callMethod<jboolean>("find");
    if (found)
    {
        auto group = matcher.callObjectMethod("group", "(I)Ljava/lang/String;", 1);
        if (_catchException(env)) return;
        auto groupStringUTF = env->NewStringUTF(group.toString().toStdString().c_str());
        uri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", groupStringUTF);
        if (_catchException(env)) return;
    }

    auto dataColumn = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;");
    auto dataColumnStringUTF = env->NewStringUTF(dataColumn.toString().toStdString().c_str());
    auto dataColumnArray = env->NewObjectArray(1, env->FindClass("java/lang/String"), NULL);
    env->SetObjectArrayElement(dataColumnArray, 0, dataColumnStringUTF);
    if (_catchException(env)) return;

    auto contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    auto cursor = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;",
                                                   uri.object<jobject>(), dataColumnArray, NULL, NULL, NULL);
    auto columnIndex = cursor.callMethod<jint>("getColumnIndexOrThrow", "(Ljava/lang/String;)I", dataColumn.object<jstring>());
    if (_catchException(env)) return;
    
    Q_TEST(cursor.callMethod<jboolean>("moveToFirst", "()Z"));
    
    auto imagePath = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex).toString();
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
