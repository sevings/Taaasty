/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDateTime>
#include <QDebug>

#include "defines.h"

#include "tasty.h"
#include "settings.h"
#include "pusherclient.h"
#include "textreader.h"
#include "cache/cachemanager.h"
#include "cache/cachedimage.h"
#include "models/feedmodel.h"
#include "models/calendarmodel.h"
#include "models/commentsmodel.h"
#include "models/attachedimagesmodel.h"
#include "models/usersmodeltlog.h"
#include "models/notificationsmodel.h"
#include "models/chatsmodel.h"
#include "models/messagesmodel.h"
#include "models/flowsmodel.h"
#include "models/tagsmodel.h"
#include "nbc/bayes.h"
#include "nbc/trainer.h"

#include "data/AttachedImage.h"
#include "data/Author.h"
#include "data/CalendarEntry.h"
#include "data/Comment.h"
#include "data/Entry.h"
#include "data/Media.h"
#include "data/Notification.h"
#include "data/Message.h"
#include "data/Conversation.h"
#include "data/Rating.h"
#include "data/Tlog.h"
#include "data/User.h"
#include "data/Flow.h"



int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setApplicationName("Taaasty");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("binque");

    QQuickStyle::setStyle("Material");

#ifdef QT_DEBUG
    auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
#endif

    qmlRegisterType<FeedModel>          ("org.binque.taaasty", 1, 0, "FeedModel");
    qmlRegisterType<CalendarModel>      ("org.binque.taaasty", 1, 0, "CalendarModel");
    qmlRegisterType<CommentsModel>      ("org.binque.taaasty", 1, 0, "CommentsModel");
    qmlRegisterType<AttachedImagesModel>("org.binque.taaasty", 1, 0, "AttachedImagesModel");
    qmlRegisterType<UsersModelTlog>     ("org.binque.taaasty", 1, 0, "UsersModelTlog");
    qmlRegisterType<MessagesModel>      ("org.binque.taaasty", 1, 0, "MessagesModel");
    qmlRegisterType<FlowsModel>         ("org.binque.taaasty", 1, 0, "FlowsModel");
    qmlRegisterType<TagsModel>          ("org.binque.taaasty", 1, 0, "TagsModel");

    qmlRegisterUncreatableType<UsersModel>("org.binque.taaasty", 1, 0, "UsersModel", "Use subclasses instead");

    qmlRegisterType<Entry>          ("org.binque.taaasty", 1, 0, "TlogEntry");
    qmlRegisterType<CalendarEntry>  ("org.binque.taaasty", 1, 0, "CalendarEntry");
    qmlRegisterType<Comment>        ("org.binque.taaasty", 1, 0, "Comment");
    qmlRegisterType<User>           ("org.binque.taaasty", 1, 0, "User");
    qmlRegisterType<Author>         ("org.binque.taaasty", 1, 0, "Author");
    qmlRegisterType<Tlog>           ("org.binque.taaasty", 1, 0, "Tlog");
    qmlRegisterType<Rating>         ("org.binque.taaasty", 1, 0, "Rating");
    qmlRegisterType<AttachedImage>  ("org.binque.taaasty", 1, 0, "AttachedImage");
    qmlRegisterType<Media>          ("org.binque.taaasty", 1, 0, "Media");
    qmlRegisterType<Notification>   ("org.binque.taaasty", 1, 0, "Notification");
    qmlRegisterType<Message>        ("org.binque.taaasty", 1, 0, "Message");
    qmlRegisterType<MessageBase>    ("org.binque.taaasty", 1, 0, "MessageBase");
    qmlRegisterType<Conversation>   ("org.binque.taaasty", 1, 0, "Chat");
    qmlRegisterType<Flow>           ("org.binque.taaasty", 1, 0, "Flow");

    qmlRegisterType<CachedImage>("ImageCache", 2, 0, "CachedImage");

    qmlRegisterType<TextReader>("TextReader", 1, 0, "TextReader");

    QQmlApplicationEngine engine;
    auto web = engine.networkAccessManager();
    auto tasty = Tasty::instance(web);
    engine.rootContext()->setContextProperty("Tasty", tasty);

    auto settings = tasty->settings();
    engine.rootContext()->setContextProperty("Settings", settings);

    auto notifs = NotificationsModel::instance(tasty);
    engine.rootContext()->setContextProperty("NotifsModel", notifs);

    auto chats = ChatsModel::instance(tasty);
    engine.rootContext()->setContextProperty("ChatsModel", chats);

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
    int density = 160; //! \todo: why 160?
#else
    float density = 267; // test
#endif

    double scale = density < 180 ? 1 :
                   density < 270 ? 1.5 :
                   density < 360 ? 2 : 3;

    engine.rootContext()->setContextProperty("mm", density / 25.4); // N900: 1 mm = 10.5 px; Q10: 12.9
    engine.rootContext()->setContextProperty("pt", 1);
    engine.rootContext()->setContextProperty("dp", scale); // N900: 1.5; Q10: 2

//    engine.rootContext()->setContextProperty("qtVersion", QT_VERSION_STR);
    engine.rootContext()->setContextProperty("builtAt", QDate::currentDate().toString("dd.MM.yy"));

    engine.setBaseUrl(QStringLiteral("qrc:/qml/"));
    engine.load(QUrl(QStringLiteral("main.qml")));

#ifdef QT_DEBUG
    auto ms = QDateTime::currentDateTime().toMSecsSinceEpoch() - now;
    qDebug() << "Started in" << ms << "ms";
#endif

    int res = app.exec();

    auto pusher = tasty->pusher();
    delete tasty;
    delete pusher;

    return res;
}
