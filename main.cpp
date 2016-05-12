#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#if defined(Q_OS_ANDROID)
#   include <QtAndroidExtras/QAndroidJniObject>
#else
#   include <QScreen>
#endif

#include "tasty.h"
#include "cache/cachemanager.h"
#include "cache/cachedimage.h"
#include "feedmodel.h"
#include "commentsmodel.h"
#include "attachedimagesmodel.h"
#include "usersmodel.h"
#include "notificationsmodel.h"
//#include "bayes.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Taaasty");
    app.setOrganizationName("binque");
    app.setApplicationVersion("0.1.0");

    qmlRegisterType<FeedModel>          ("org.binque.taaasty", 1, 0, "FeedModel");
    qmlRegisterType<CommentsModel>      ("org.binque.taaasty", 1, 0, "CommentsModel");
    qmlRegisterType<AttachedImagesModel>("org.binque.taaasty", 1, 0, "AttachedImagesModel");
    qmlRegisterType<UsersModel>         ("org.binque.taaasty", 1, 0, "UsersModel");
    qmlRegisterType<NotificationsModel> ("org.binque.taaasty", 1, 0, "NotificationsModel");

    qmlRegisterType<Entry>          ("org.binque.taaasty", 1, 0, "TlogEntry");
    qmlRegisterType<Comment>        ("org.binque.taaasty", 1, 0, "Comment");
    qmlRegisterType<User>           ("org.binque.taaasty", 1, 0, "User");
    qmlRegisterType<Author>         ("org.binque.taaasty", 1, 0, "Author");
    qmlRegisterType<Tlog>           ("org.binque.taaasty", 1, 0, "Tlog");
    qmlRegisterType<Rating>         ("org.binque.taaasty", 1, 0, "Rating");
    qmlRegisterType<AttachedImage>  ("org.binque.taaasty", 1, 0, "AttachedImage");
    qmlRegisterType<Notification>   ("org.binque.taaasty", 1, 0, "Notification");

    qmlRegisterType<CachedImage>("ImageCache", 2, 0, "CachedImage");

    QQmlApplicationEngine engine;
    auto tasty = Tasty::instance();
    engine.rootContext()->setContextProperty("Tasty", tasty);

    auto web = tasty->manager();
    auto settings = tasty->settings();
    auto cache = CacheManager::instance(web);
    cache->setMaxWidth(settings->maxImageWidth());
    cache->setAutoload(settings->autoloadImages());
//    cache->setAutoload(false);
    engine.rootContext()->setContextProperty("Cache", cache);
//    engine.rootContext()->setContextProperty("Bayes", Bayes::instance(Tasty::instance()));

#ifdef Q_OS_ANDROID
    //  BUG with dpi on some androids: https://bugreports.qt-project.org/browse/QTBUG-35701
    //  Workaround:
    QAndroidJniObject qtActivity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    QAndroidJniObject resources = qtActivity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
    QAndroidJniObject displayMetrics = resources.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
    int density = displayMetrics.getField<int>("densityDpi");
#else
    auto *screen = qApp->primaryScreen();
    float density = screen->physicalDotsPerInch();
    density = 267; // test
#endif

    engine.rootContext()->setContextProperty("mm",density / 25.4); // N900: 1 mm = 10.5 px; Q10: 12.9
    engine.rootContext()->setContextProperty("pt", 1);

    double scale = density < 180 ? 1 :
                   density < 270 ? 1.5 :
                   density < 360 ? 2 : 3;
    engine.rootContext()->setContextProperty("dp", scale); // N900: 1.5; Q10: 2

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    int res = app.exec();

//    delete cache;
    delete tasty;

    return res;
}
