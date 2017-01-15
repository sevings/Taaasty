#include "androidimagepicker.h"

#include <QtAndroid>
#include <QAndroidJniEnvironment>
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
    auto uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
    auto uriPath = uri.callObjectMethod("getPath", "()Ljava/lang/String;");

    qDebug() << " image uri path:" << uriPath.toString();

    auto uriNotValid = uriPath.callMethod<jboolean>("contains", "(Z)Ljava/lang/CharSequence;", env->NewStringUTF("content"));
    if (uriNotValid)
    {
        auto pattern = QAndroidJniObject::callStaticObjectMethod("java/util/regex/Pattern", "compile",
                                                                 "(Ljava/lang/String;)Ljava/util/regex/Pattern;",
                                                                 env->NewStringUTF("(content://media/.*\\d)"));
        auto uriPathStringUTF = env->NewStringUTF(uriPath.toString().toStdString().c_str());
        auto matcher = pattern.callObjectMethod("matcher", "(Ljava/lang/CharSequence;)Ljava/util/regex/Matcher;", uriPathStringUTF);
        auto found = matcher.callMethod<jboolean>("find");
        if (found)
        {
            auto group = matcher.callObjectMethod("group", "(I)Ljava/lang/String;");
            auto groupStringUTF = env->NewStringUTF(group.toString().toStdString().c_str());
            uri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", groupStringUTF);
        }
        else
        {
            qDebug() << "Uri path is not valid: " << uriPath.toString();
            Q_TEST(QMetaObject::invokeMethod(pTasty, "error", Q_ARG(int, 0), Q_ARG(QString, "Ошибка при открытии изображения")));
            return;
        }
    }

    auto dadosAndroid = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;");

    auto projecao = env->NewObjectArray(1, env->FindClass("java/lang/String"), NULL);
    auto projacaoDadosAndroid = env->NewStringUTF(dadosAndroid.toString().toStdString().c_str());
    env->SetObjectArrayElement(projecao, 0, projacaoDadosAndroid);

    auto contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    auto cursor = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;",
                                                   uri.object<jobject>(), projecao, NULL, NULL, NULL);
    auto columnIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", dadosAndroid.object<jstring>());
    cursor.callMethod<jboolean>("moveToFirst", "()Z");

    auto image = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex).toString();

    Q_TEST(QMetaObject::invokeMethod(_parent, "_append", Qt::QueuedConnection, Q_ARG(QString, image)));
}
