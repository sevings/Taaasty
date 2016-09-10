#include "tasty.h"

#include "apirequest.h"
#include "pusherclient.h"

#include "data/User.h"

#include "nbc/bayes.h"

#include "defines.h"

#include <QGuiApplication>
#include <QDateTime>
#include <QRegularExpression>
#include <QDebug>



Tasty::Tasty(QNetworkAccessManager* web)
    : QObject()
    , _settings(new Settings(this))
    , _manager(web ? web : new QNetworkAccessManager(this))
    , _pusher(new PusherClient(this))
    , _entryImageWidth(_settings->maxImageWidth())
    , _commentImageWidth(_entryImageWidth)
    , _unreadChats(0)
    , _unreadNotifications(0)
    , _me(nullptr)
{
    qDebug() << "Tasty";

    Q_TEST(connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                   this, SLOT(_saveOrReconnect(Qt::ApplicationState))));

    Q_TEST(connect(_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
                   this, SLOT(_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility))));

    Q_TEST(connect(_pusher, SIGNAL(unreadChats(int)),         this, SLOT(_setUnreadChats(int))));
    Q_TEST(connect(_pusher, SIGNAL(unreadNotifications(int)), this, SLOT(_setUnreadNotifications(int))));

//    _pusher->subscribe("live");
}



Tasty::~Tasty()
{
    delete _settings;
}



Tasty* Tasty::instance(QNetworkAccessManager* web)
{
    static auto tasty = new Tasty(web);
    return tasty;
}



bool Tasty::isAuthorized() const
{
    return !_settings->accessToken().isEmpty();
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



void Tasty::authorize(const QString login, const QString password)
{
    qDebug() << "authorize";

    auto data = QString("email=%1&password=%2")
            .arg(login)
            .arg(password);

    auto request = new ApiRequest("v1/sessions.json", false,
                                  QNetworkAccessManager::PostOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));
}



void Tasty::reconnectToPusher()
{
    _pusher->connect();
}



void Tasty::_readAccessToken(const QJsonObject data)
{
    auto apiKey      = data.value("api_key").toObject();
    auto accessToken = apiKey.value("access_token").toString();
    auto expiresAt   = apiKey.value("expires_at").toString();
    auto userId      = apiKey.value("user_id").toInt();
    auto login       = data.value("slug").toString();

    _settings->setAccessToken(accessToken);
    _settings->setExpiresAt(expiresAt);
    _settings->setUserId(userId);
    _settings->setLogin(login);

    _unreadChats = 0;
    emit unreadChatsChanged();

    _unreadNotifications = 0;
    emit unreadNotificationsChanged();

    if (_me)
        _me->setId(userId);

    emit authorized();
}



void Tasty::_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (accessible == QNetworkAccessManager::NotAccessible)
        emit error(0, "Сеть недоступна");
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



void Tasty::_saveOrReconnect(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive)
        _pusher->connect();
    else
        Bayes::instance()->saveDb();
}
