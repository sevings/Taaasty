#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "tasty.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Taaasty");
    app.setOrganizationName("binque");

    QQmlApplicationEngine engine;

    auto root = engine.rootContext();
    auto tasty = Tasty::instance();
    root->setContextProperty("tasty", tasty);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    tasty->getMe();

    auto res = app.exec();

    delete tasty;

    return res;
}
