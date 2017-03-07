// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDateTime>
#include <QDebug>

#include "tasty.h"
#include "apirequest.h"
#include "settings.h"
#include "pusherclient.h"
#include "texthandler.h"
#include "poster.h"
#include "reposter.h"
#include "floweditor.h"
#include "tastydatacache.h"
#include "cache/cachemanager.h"
#include "cache/cachedimage.h"
#include "cache/cachedimageprovider.h"
#include "models/feedmodel.h"
#include "models/calendarmodel.h"
#include "models/commentsmodel.h"
#include "models/attachedimagesmodel.h"
#include "models/usersmodel.h"
#include "models/notificationsmodel.h"
#include "models/chatsmodel.h"
#include "models/messagesmodel.h"
#include "models/flowsmodel.h"
#include "models/availabletlogsmodel.h"
#include "models/tagsmodel.h"
#include "models/uploadmodel.h"

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

QRegularExpression Tasty::_firstSlugRe(QStringLiteral("^(~|@)([a-zA-Z0-9_\\-\\.]+)"));
QRegularExpression Tasty::_slugRe(QStringLiteral("([^'/>\\w\\-\\.])(~|@)([a-zA-Z0-9_\\-\\.]+)"));
QRegularExpression Tasty::_imageLinkRe(QStringLiteral("<a[^>]*>(<img[^>]*>)</a>"));
QRegularExpression Tasty::_imgRe("<img (?:width=\"\\d+\" )?");
QRegularExpression Tasty::_tagRe(QStringLiteral("<[^>]*>"));



Tasty::Tasty()
    : QObject()
    , _engine(nullptr)
    , _settings(nullptr)
    , _manager(nullptr)
    , _pusher(nullptr)
    , _dataCache(nullptr)
    , _entryImageWidth(0)
    , _commentImageWidth(0)
    , _unreadChats(0)
    , _unreadNotifications(0)
    , _me(nullptr)
    , _saveProfile(false)
{
    qDebug() << "Tasty";

    _firstSlugRe.optimize();
    _slugRe.optimize();
    _imageLinkRe.optimize();
    _imgRe.optimize();
    _tagRe.optimize();

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



QList<EntryPtr> Tasty::clearUnreadFriendsEntries()
{
    auto list = _unreadFriendsEntries;

    _unreadFriendsEntries.clear();
    emit unreadFriendsEntriesChanged();

    return list;
}



QString Tasty::num2str(const int n, const QString& str1, const QString& str234, const QString& str5)
{
    QString res = QStringLiteral("%1 %2").arg(n);
    if (n % 10 == 1 && n % 100 != 11)
        res = res.arg(str1);
    else if ((n % 10 > 1 && n % 10 < 5) && (n % 100 < 10 || n % 100 > 20))
        res = res.arg(str234);
    else
        res = res.arg(str5);
    return res;
}



QString Tasty::parseDate(const QString& d, const bool bigLetter)
{
    auto datetime = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
    auto date = datetime.date();
    auto today = QDate::currentDate();

    if (today == date)
        return datetime.toString(QStringLiteral("%1егодня в H:mm").arg(bigLetter ? "С" : "с"));
    if (today == date.addDays(1))
        return datetime.toString(QStringLiteral("%1чера в H:mm").arg(bigLetter ? "В" : "в"));

    bool showYear = date.year() != today.year();
    QString format = showYear ? "d MMM yyyy" : "d MMM в H:mm";
    return datetime.toString(format);
}



void Tasty::correctHtml(QString& html, bool isEntry)
{
    html.replace(_firstSlugRe, "<a href='http://taaasty.com/~\\2'>\\1\\2</a>");
    html.replace(_slugRe, "\\1<a href='http://taaasty.com/~\\3'>\\2\\3</a>");
    html.replace(_imageLinkRe, "\\1");

//    html.replace("class=\"embeded__image\"", QString());

    auto width = isEntry ? pTasty->_entryImageWidth
                         : pTasty->_commentImageWidth;

    html.replace(_imgRe, QString("<img width=\"%1\" ").arg(width));
}



QString Tasty::truncateHtml(QString html, int length)
{
    html.remove(_tagRe).replace('\n', ' ').truncate(length);
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



void Tasty::authorize(const QString& login, const QString& password, bool save)
{
    qDebug() << "authorize";

    _saveProfile = save;

    auto request = new ApiRequest("v1/sessions.json", ApiRequest::ShowMessageOnError);
    request->addFormData("email", login);
    request->addFormData("password", password);
    request->post();

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));

}



void Tasty::logout()
{
    if (!isAuthorized())
        return;

    qDebug() << "log out";

    _settings->clearProfile();
    emit authorizedChanged();
}



void Tasty::swapProfiles()
{
    auto token = _settings->prevAccessToken();
    if (token.isEmpty())
        return;

    // avoid error 403 reasonable_security_violation (is that even legal?)
    auto request = new ApiRequest("v1/app/stats.json", token);
    request->get();

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
    _dataCache  = new TastyDataCache;

    for (int i = 0; i < 6; i++)
    {
        _manager->connectToHost(QStringLiteral("http://api.taaasty.com"));
        _manager->connectToHost(QStringLiteral("http://thumbor4.tasty0.ru"));
        _manager->connectToHostEncrypted(QStringLiteral("https://tasty-prod.s3.amazonaws.com"));
    }

    _entryImageWidth   = _settings->maxImageWidth();
    _commentImageWidth = _entryImageWidth;

    Q_TEST(connect(qApp, &QGuiApplication::applicationStateChanged, this, &Tasty::_saveOrReconnect, Qt::QueuedConnection));

    Q_TEST(connect(_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
                   this, SLOT(_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility))));

    Q_TEST(connect(_pusher, &PusherClient::unreadChats,          this, &Tasty::_setUnreadChats));
    Q_TEST(connect(_pusher, &PusherClient::unreadNotifications,  this, &Tasty::_setUnreadNotifications));
    Q_TEST(connect(_pusher, &PusherClient::unreadFriendsEntry,   this, &Tasty::_addUnreadFriendEntry));

    QQuickStyle::setStyle(QStringLiteral("Material"));

    qmlRegisterType<FeedModel>          ("org.binque.taaasty", 1, 0, "FeedModel");
    qmlRegisterType<CalendarModel>      ("org.binque.taaasty", 1, 0, "CalendarModel");
    qmlRegisterType<CommentsModel>      ("org.binque.taaasty", 1, 0, "CommentsModel");
    qmlRegisterType<AttachedImagesModel>("org.binque.taaasty", 1, 0, "AttachedImagesModel");
    qmlRegisterType<UsersModel>         ("org.binque.taaasty", 1, 0, "UsersModel");
    qmlRegisterType<MessagesModel>      ("org.binque.taaasty", 1, 0, "MessagesModel");
    qmlRegisterType<FlowsModel>         ("org.binque.taaasty", 1, 0, "FlowsModel");
    qmlRegisterType<AvailableTlogsModel>("org.binque.taaasty", 1, 0, "AvailableTlogsModel");
    qmlRegisterType<TagsModel>          ("org.binque.taaasty", 1, 0, "TagsModel");
    qmlRegisterType<UploadModel>        ("org.binque.taaasty", 1, 0, "UploadModel");

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
    qmlRegisterType<Reposter>       ("org.binque.taaasty", 1, 0, "Reposter");
    qmlRegisterType<FlowEditor>     ("org.binque.taaasty", 1, 0, "FlowEditor");

    qmlRegisterType<CachedImage>("ImageCache", 2, 0, "CachedImage");

    auto root = _engine->rootContext();
    root->setContextProperty("Tasty", this);
    root->setContextProperty("Settings", _settings);

    auto notifs = NotificationsModel::instance(this);
    root->setContextProperty("NotifsModel", notifs);

    auto friendActivity = NotificationsModel::friendActivity(this);
    root->setContextProperty("FriendActivityModel", friendActivity);

    auto chats = ChatsModel::instance(this);
    root->setContextProperty("ChatsModel", chats);

    auto cache = CacheManager::instance(_settings->maxCacheSize(), _manager);
    cache->setMaxWidth(_settings->maxImageWidth());
    cache->setAutoloadOverWifi(_settings->loadImagesOverWifi());
    cache->setMaxLoadSize(_settings->maxLoadImageSize());
    root->setContextProperty("Cache", cache);

    if (_settings->cacheVersion() < 2)
    {
        cache->clearOldVersion();
        Q_TEST(connect(cache, &CacheManager::oldVersionCleared, []()
        {
            pTasty->settings()->setCacheVersion(2);
        }));
    }

    _engine->addImageProvider("cached", cache->provider());

    Q_TEST(QObject::connect(_settings, SIGNAL(loadImagesOverWifiChanged(bool)), cache, SLOT(setAutoloadOverWifi(bool))));
    Q_TEST(QObject::connect(_settings, SIGNAL(maxLoadImageSizeChanged(int)),    cache, SLOT(setMaxLoadSize(int))));
    Q_TEST(QObject::connect(_settings, SIGNAL(maxCacheSizeChanged(int)),        cache, SLOT(setMaxSize(int))));

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
    auto request = qobject_cast<ApiRequest*>(sender());
    if (request)
    {
        disconnect(request, SIGNAL(error(int,QString)),   this, SLOT(_swapProfiles()));
        disconnect(request, SIGNAL(success(QJsonObject)), this, SLOT(_swapProfiles()));
    }

    _settings->swapProfiles();

    _finishLogin();
}



void Tasty::_readAccessToken(const QJsonObject& data)
{
    if (_settings->saveProfile())
        _settings->swapProfiles();

    auto apiKey      = data.value(QStringLiteral("api_key")).toObject();
    auto accessToken = apiKey.value(QStringLiteral("access_token")).toString();
    auto expiresAt   = apiKey.value(QStringLiteral("expires_at")).toString();
    auto userId      = apiKey.value(QStringLiteral("user_id")).toInt();
    auto login       = data.value(QStringLiteral("slug")).toString();

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



void Tasty::_addUnreadFriendEntry(int entryId)
{
    auto entry = _dataCache->entry(entryId);
    if (!entry)
    {
        entry = EntryPtr::create(nullptr);
        entry->setId(entryId);
    }

    _unreadFriendsEntries << entry;
    emit unreadFriendsEntriesChanged();
}



void Tasty::_saveOrReconnect(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive)
    {
        if (_pusher->isConnected())
            return;

        _pusher->connect();

        ChatsModel::instance()->loadLast();
        NotificationsModel::instance()->check();
    }
    else
    {
        CacheManager::instance()->saveDb();
        Bayes::instance()->saveDb();
    }
}



void Tasty::_finishLogin()
{
    _unreadChats = 0;
    emit unreadChatsChanged();

    _unreadNotifications = 0;
    emit unreadNotificationsChanged();

    _unreadFriendsEntries.clear();
    emit unreadFriendsEntriesChanged();

    if (_me)
        _me->setId(_settings->userId());

    emit authorizedChanged();
}
