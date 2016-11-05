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

#include "tasty.h"

#include <QGuiApplication>
#include <QRegularExpression>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDateTime>
#include <QDebug>

#include "defines.h"

#include "tasty.h"
#include "apirequest.h"
#include "settings.h"
#include "pusherclient.h"
#include "texthandler.h"
#include "poster.h"
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



Tasty::Tasty()
    : QObject()
    , _engine(nullptr)
    , _settings(nullptr)
    , _manager(nullptr)
    , _pusher(nullptr)
    , _entryImageWidth(0)
    , _commentImageWidth(0)
    , _unreadChats(0)
    , _unreadNotifications(0)
    , _unreadFriendsEntries(0)
    , _me(nullptr)
    , _saveProfile(false)
{
    qDebug() << "Tasty";

    Q_TEST(QMetaObject::invokeMethod(this, "_init"));
}



Tasty::~Tasty()
{
    delete _settings;
}



Tasty* Tasty::instance()
{
    static auto tasty = new Tasty;
    return tasty;
}



bool Tasty::isAuthorized() const
{
    return !_settings->accessToken().isEmpty();
}



void Tasty::clearUnreadFriendsEntries()
{
    _unreadFriendsEntries = 0;
    emit unreadFriendsEntriesChanged();
}



QString Tasty::num2str(const int n, const QString str1, const QString str234, const QString str5)
{
    QString res = QString("%1 %2").arg(n);
    if (n % 10 == 1 && n % 100 != 11)
        res = res.arg(str1);
    else if ((n % 10 > 1 && n % 10 < 5) && (n % 100 < 10 || n % 100 > 20))
        res = res.arg(str234);
    else
        res = res.arg(str5);
    return res;
}



QString Tasty::parseDate(const QString d, const bool bigLetter)
{
    auto datetime = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
    auto date = datetime.date();
    auto today = QDate::currentDate();

    if (today == date)
        return datetime.toString(QString("%1егодня в H:mm").arg(bigLetter ? "С" : "с"));
    if (today == date.addDays(1))
        return datetime.toString(QString("%1чера в H:mm").arg(bigLetter ? "В" : "в"));

    bool showYear = date.year() != today.year();
    QString format = showYear ? "d MMM yyyy" : "d MMM в H:mm";
    return datetime.toString(format);
}



void Tasty::correctHtml(QString& html, bool isEntry)
{
    QRegularExpression firstSlugRe("^(~|@)([a-zA-Z0-9_\\-\\.]+)");
    html.replace(firstSlugRe, "<a href='http://taaasty.com/~\\2'>\\1\\2</a>");

    QRegularExpression slugRe("([^'/>\\w\\-\\.])(~|@)([a-zA-Z0-9_\\-\\.]+)");
    html.replace(slugRe, "\\1<a href='http://taaasty.com/~\\3'>\\2\\3</a>");

    QRegularExpression imageLinkRe("<a[^>]*>(<img[^>]*>)</a>");
    html.replace(imageLinkRe, "\\1");
    
    auto width = isEntry ? Tasty::instance()->_entryImageWidth
                         : Tasty::instance()->_commentImageWidth;
    QRegularExpression imgRe("<img (?:width=\\d+ )?");
    html.replace(imgRe, QString("<img width=%1 ").arg(width));
}



QString Tasty::truncateHtml(QString html, int length)
{
    html.remove(QRegularExpression("<[^>]*>"))
            .replace('\n', ' ').truncate(length);
    return html;
}



void Tasty::setImageWidth(int entry, int comment)
{
    if (entry <= 0 || entry == _entryImageWidth
            || comment <= 0 || comment == _commentImageWidth)
        return;

    _entryImageWidth = entry;
    _commentImageWidth = comment;

    emit htmlRecorrectionNeeded();
}



User* Tasty::me()
{
    if (_me)
        return _me;

    if (!isAuthorized())
        return nullptr;

    _me = new User(this);
    _me->setId(_settings->userId());
    return _me;
}



void Tasty::authorize(const QString login, const QString password, bool save)
{
    qDebug() << "authorize";

    _saveProfile = save;

    auto data = QString("email=%1&password=%2")
            .arg(login)
            .arg(password);

    auto request = new ApiRequest("v1/sessions.json", ApiRequest::ShowMessageOnError,
                                  QNetworkAccessManager::PostOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));

}



void Tasty::swapProfiles()
{
    auto token = _settings->prevAccessToken();
    if (token.isEmpty())
        return;

    // avoid error 403 reasonable_security_violation (is that even legal?)
    auto request = new ApiRequest("v1/app/stats.json", token);
    Q_TEST(connect(request, SIGNAL(error(int,QString)),   this, SLOT(_swapProfiles())));
    Q_TEST(connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_swapProfiles())));
}



void Tasty::reconnectToPusher()
{
    _pusher->connect();
}



void Tasty::_init()
{
#ifdef QT_DEBUG
    static bool inited = false;
    Q_ASSERT(!inited);
    inited = true;

    auto now = QDateTime::currentMSecsSinceEpoch();
#endif

    Bayes::instance(this);

    _engine     = new QQmlApplicationEngine(this);
    _settings   = new Settings(this);
    _manager    = _engine->networkAccessManager();
    _pusher     = new PusherClient(this);

    _entryImageWidth   = _settings->maxImageWidth();
    _commentImageWidth = _entryImageWidth;

    Q_TEST(connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                   this, SLOT(_saveOrReconnect(Qt::ApplicationState))));

    Q_TEST(connect(_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
                   this, SLOT(_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility))));

    Q_TEST(connect(_pusher, SIGNAL(unreadChats(int)),          this, SLOT(_setUnreadChats(int))));
    Q_TEST(connect(_pusher, SIGNAL(unreadNotifications(int)),  this, SLOT(_setUnreadNotifications(int))));
    Q_TEST(connect(_pusher, SIGNAL(unreadFriendsEntry(int)), this, SLOT(_incUnreadFriendsEntries())));

    QQuickStyle::setStyle("Material");

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

    qmlRegisterType<TextHandler>    ("org.binque.taaasty", 1, 0, "TextHandler");
    qmlRegisterType<Poster>         ("org.binque.taaasty", 1, 0, "Poster");

    qmlRegisterType<CachedImage>("ImageCache", 2, 0, "CachedImage");

    auto root = _engine->rootContext();
    root->setContextProperty("Tasty", this);
    root->setContextProperty("Settings", _settings);

    auto notifs = NotificationsModel::instance(this);
    root->setContextProperty("NotifsModel", notifs);

    auto chats = ChatsModel::instance(this);
    root->setContextProperty("ChatsModel", chats);

    auto cache = CacheManager::instance(_manager);
    cache->setMaxWidth(_settings->maxImageWidth());
    cache->setAutoload(_settings->autoloadImages());
    root->setContextProperty("Cache", cache);

    Q_TEST(QObject::connect(_settings, SIGNAL(autoloadImagesChanged(bool)), cache, SLOT(setAutoload(bool))));

    auto bayes = Bayes::instance();
    root->setContextProperty("Bayes", bayes);

    auto trainer = bayes->trainer();
    root->setContextProperty("Trainer", trainer);

#ifdef Q_OS_ANDROID
    float density = 160;
#else
    float density = 267; // test
#endif

    double scale = density < 180 ? 1 :
                   density < 270 ? 1.5 :
                   density < 360 ? 2 : 3;

    root->setContextProperty("mm", density / 25.4); // N900: 1 mm = 10.5 px; Q10: 12.9
    root->setContextProperty("pt", 1);
    root->setContextProperty("dp", scale); // N900: 1.5; Q10: 2
    root->setContextProperty("sp", density / 160); // scaleable pixels

    root->setContextProperty("builtAt", QString::fromLatin1(__DATE__));

    _engine->setBaseUrl(QStringLiteral("qrc:/qml/"));
    _engine->load(QUrl(QStringLiteral("main.qml")));

#ifdef QT_DEBUG
    auto ms = QDateTime::currentMSecsSinceEpoch() - now;
    qDebug() << "Started in" << ms << "ms";
#endif
}



void Tasty::_swapProfiles()
{
    _settings->swapProfiles();

    _finishLogin();
}



void Tasty::_readAccessToken(const QJsonObject data)
{
    if (_settings->saveProfile())
        _settings->swapProfiles();

    auto apiKey      = data.value("api_key").toObject();
    auto accessToken = apiKey.value("access_token").toString();
    auto expiresAt   = apiKey.value("expires_at").toString();
    auto userId      = apiKey.value("user_id").toInt();
    auto login       = data.value("slug").toString();

    _settings->setAccessToken(accessToken);
    _settings->setExpiresAt(expiresAt);
    _settings->setUserId(userId);
    _settings->setLogin(login);
    _settings->setSaveProfile(_saveProfile);

    _finishLogin();
}



void Tasty::_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (accessible == QNetworkAccessManager::NotAccessible)
        emit networkNotAccessible();
    else if (accessible == QNetworkAccessManager::Accessible)
        emit networkAccessible();
}



void Tasty::_setUnreadChats(int count)
{
    if (count == _unreadChats)
        return;

    _unreadChats = count;
    emit unreadChatsChanged();
}



void Tasty::_setUnreadNotifications(int count)
{
    if (count == _unreadNotifications)
        return;

    _unreadNotifications = count;
    emit unreadNotificationsChanged();
}



void Tasty::_incUnreadFriendsEntries()
{
    _unreadFriendsEntries++;
    emit unreadFriendsEntriesChanged();
}



void Tasty::_saveOrReconnect(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive)
        _pusher->connect();
    else
        Bayes::instance()->saveDb();
}



void Tasty::_finishLogin()
{
    _unreadChats = 0;
    emit unreadChatsChanged();

    _unreadNotifications = 0;
    emit unreadNotificationsChanged();

    _unreadFriendsEntries = 0;
    emit unreadFriendsEntriesChanged();
    
    if (_me)
        _me->setId(_settings->userId());

    emit authorized();
}
