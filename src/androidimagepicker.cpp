#include "androidimagepicker.h"

#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QDebug>



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

    auto uri = data.callObjectMethod("getData", "()Landroid/net/Uri;");
    auto dadosAndroid = QAndroidJniObject::getStaticObjectField("android/provider/MediaStore$MediaColumns", "DATA", "Ljava/lang/String;");

    QAndroidJniEnvironment env;
    auto projecao = env->NewObjectArray(1, env->FindClass("java/lang/String"), NULL);
    auto projacaoDadosAndroid = env->NewStringUTF(dadosAndroid.toString().toStdString().c_str());
    env->SetObjectArrayElement(projecao, 0, projacaoDadosAndroid);

    auto contentResolver = QtAndroid::androidActivity().callObjectMethod("getContentResolver", "()Landroid/content/ContentResolver;");
    auto cursor = contentResolver.callObjectMethod("query", "(Landroid/net/Uri;[Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;Ljava/lang/String;)Landroid/database/Cursor;", uri.object<jobject>(), projecao, NULL, NULL, NULL);
    auto columnIndex = cursor.callMethod<jint>("getColumnIndex", "(Ljava/lang/String;)I", dadosAndroid.object<jstring>());
    cursor.callMethod<jboolean>("moveToFirst", "()Z");

    auto image = cursor.callObjectMethod("getString", "(I)Ljava/lang/String;", columnIndex).toString();

    Q_TEST(QMetaObject::invokeMethod(_parent, "_append", Qt::QueuedConnection, Q_ARG(QString, image)));
}
