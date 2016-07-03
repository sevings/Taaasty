#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

#if defined(Q_OS_ANDROID)
#   include <QtAndroidExtras/QAndroidJniObject>
#else
#   include <QScreen>
#endif

#include "defines.h"

#include "tasty.h"
#include "settings.h"
#include "cache/cachemanager.h"
#include "cache/cachedimage.h"
#include "models/feedmodel.h"
#include "models/commentsmodel.h"
#include "models/attachedimagesmodel.h"
#include "models/usersmodeltlog.h"
#include "models/usersmodelbayes.h"
#include "models/notificationsmodel.h"
#include "nbc/bayes.h"
#include "nbc/trainer.h"

#include "data/AttachedImage.h"
#include "data/Author.h"
#include "data/CalendarEntry.h"
#include "data/Comment.h"
#include "data/Entry.h"
#include "data/Media.h"
#include "data/Notification.h"
#include "data/Rating.h"
#include "data/Tlog.h"
#include "data/User.h"

#include "messageoutput.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Taaasty");
    app.setOrganizationName("binque");

//    installLog();

    qmlRegisterType<FeedModel>          ("org.binque.taaasty", 1, 0, "FeedModel");
    qmlRegisterType<CommentsModel>      ("org.binque.taaasty", 1, 0, "CommentsModel");
    qmlRegisterType<AttachedImagesModel>("org.binque.taaasty", 1, 0, "AttachedImagesModel");
    qmlRegisterType<UsersModelTlog>     ("org.binque.taaasty", 1, 0, "UsersModelTlog");
    qmlRegisterType<UsersModelBayes>    ("org.binque.taaasty", 1, 0, "UsersModelBayes");

    qmlRegisterUncreatableType<UsersModel>("org.binque.taaasty", 1, 0, "UsersModel", "Use subclasses instead");

    qmlRegisterType<Entry>          ("org.binque.taaasty", 1, 0, "TlogEntry");
    qmlRegisterType<Comment>        ("org.binque.taaasty", 1, 0, "Comment");
    qmlRegisterType<User>           ("org.binque.taaasty", 1, 0, "User");
    qmlRegisterType<Author>         ("org.binque.taaasty", 1, 0, "Author");
    qmlRegisterType<Tlog>           ("org.binque.taaasty", 1, 0, "Tlog");
    qmlRegisterType<Rating>         ("org.binque.taaasty", 1, 0, "Rating");
    qmlRegisterType<AttachedImage>  ("org.binque.taaasty", 1, 0, "AttachedImage");
    qmlRegisterType<Media>          ("org.binque.taaasty", 1, 0, "Media");
    qmlRegisterType<Notification>   ("org.binque.taaasty", 1, 0, "Notification");

    qmlRegisterType<CachedImage>("ImageCache", 2, 0, "CachedImage");

    QQmlApplicationEngine engine;
    auto tasty = Tasty::instance();
    engine.rootContext()->setContextProperty("Tasty", tasty);

    auto settings = tasty->settings();
    engine.rootContext()->setContextProperty("Settings", settings);

    auto notifs = NotificationsModel::instance(tasty);
    engine.rootContext()->setContextProperty("NotifsModel", notifs);

    auto web = tasty->manager();
    auto cache = CacheManager::instance(web);
    cache->setMaxWidth(settings->maxImageWidth());
    cache->setAutoload(settings->autoloadImages());
    engine.rootContext()->setContextProperty("Cache", cache);

    Q_TEST(QObject::connect(settings, SIGNAL(autoloadImagesChanged(bool)), cache, SLOT(setAutoload(bool))));

    auto bayes = Bayes::instance(tasty);
    engine.rootContext()->setContextProperty("Bayes", bayes);

    auto trainer = bayes->trainer();
    engine.rootContext()->setContextProperty("Trainer", trainer);

#ifdef Q_OS_ANDROID
    //  BUG with dpi on some androids: https://bugreports.qt-project.org/browse/QTBUG-35701
    //  Workaround:
    auto qtActivity = QAndroidJniObject::callStaticObjectMethod("org/qtproject/qt5/android/QtNative", "activity", "()Landroid/app/Activity;");
    auto resources = qtActivity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
    auto displayMetrics = resources.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
    int density = displayMetrics.getField<int>("densityDpi");
#else
//    auto *screen = qApp->primaryScreen();
//    float density = screen->physicalDotsPerInch();
    float density = 267; // test
#endif

    engine.rootContext()->setContextProperty("mm",density / 25.4); // N900: 1 mm = 10.5 px; Q10: 12.9
    engine.rootContext()->setContextProperty("pt", 1);

    double scale = density < 180 ? 1 :
                   density < 270 ? 1.5 :
                   density < 360 ? 2 : 3;
    engine.rootContext()->setContextProperty("dp", scale); // N900: 1.5; Q10: 2

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    int res = app.exec();

    delete tasty;

//    finishLog();

    return res;
}