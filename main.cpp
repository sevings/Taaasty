#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "tasty.h"
#include "imagecache.h"
#include "feedmodel.h"
#include "commentsmodel.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Taaasty");
    app.setOrganizationName("binque");

    qmlRegisterType<FeedModel>("org.binque.taaasty", 1, 0, "FeedModel");
    qmlRegisterType<CommentsModel>("org.binque.taaasty", 1, 0, "CommentsModel");
    qmlRegisterType<Entry>("org.binque.taaasty", 1, 0, "TlogEntry");
    qmlRegisterType<Rating>("org.binque.taaasty", 1, 0, "Rating");

    qmlRegisterType<ImageCache>("ImageCache", 1, 0, "ImageCache");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("Tasty", Tasty::instance());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    int res = app.exec();

    delete Tasty::instance();

    return res;
}
