#include "tasty.h"

#include "apirequest.h"
#include "pusherclient.h"

#include "data/Tlog.h"

#include "defines.h"

#include <QDateTime>
#include <QRegularExpression>
#include <QDebug>



Tasty::Tasty(QNetworkAccessManager* web)
    : QObject()
    , _settings(new Settings(this))
    , _manager(web ? web : new QNetworkAccessManager(this))
    , _pusher(new PusherClient(this))
    , _busy(0)
    , _entryImageWidth(_settings->maxImageWidth())
    , _commentImageWidth(_entryImageWidth)
    , _unreadChats(0)
    , _me(nullptr)
{
    qDebug() << "Tasty";

    Q_TEST(connect(_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
                   this, SLOT(_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility))));

    Q_TEST(connect(_pusher, SIGNAL(unreadChats(int)), this, SLOT(_setUnreadChats(int))));

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



void Tasty::incBusy()
{
    _busy++;
    emit busyChanged();
}



void Tasty::decBusy()
{
    if (_busy > 0)
    {
        _busy--;
        emit busyChanged();
    }
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
    QRegularExpression slugRe("([^'/>\\w\\-\\.])(~|@)([a-zA-Z0-9_\\-\\.]+)");
    html.replace(slugRe, "\\1<a href='http://taaasty.com/~\\3'>\\2\\3</a>");

    QRegularExpression imageLinkRe("<a[^>]*>(<img[^>]*>)</a>");
    html.replace(imageLinkRe, "\\1");
    
    auto width = isEntry ? Tasty::instance()->_entryImageWidth
                         : Tasty::instance()->_commentImageWidth;
    QRegularExpression imgRe("<img (?:width=\\d+ )?");
    html.replace(imgRe, QString("<img width=%1 ").arg(width));
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



Tlog* Tasty::me()
{
    if (_me)
        return _me;

    if (!isAuthorized())
        return nullptr;

    _me = new Tlog(this);
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



void Tasty::getMe()
{
    auto request = new ApiRequest("v1/users/me.json", true);
    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readMe(const QJsonObject)));
}



void Tasty::_readAccessToken(const QJsonObject data)
{
    auto apiKey      = data.value("api_key").toObject();
    auto accessToken = apiKey.value("access_token").toString();
    auto expiresAt   = apiKey.value("expires_at").toString();
    auto userId      = apiKey.value("user_id").toInt();
    auto login       = data.value("name").toString();

    _settings->setAccessToken(accessToken);
    _settings->setExpiresAt(expiresAt);
    _settings->setUserId(userId);
    _settings->setLogin(login);

    emit authorized();
}



void Tasty::_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (accessible == QNetworkAccessManager::NotAccessible)
        emit error(0, "Сеть недоступна");
    else if (accessible == QNetworkAccessManager::Accessible)
        emit networkAccessible();
}



void Tasty::_readMe(const QJsonObject data)
{
    qDebug() << data.value("title").toString();
}



void Tasty::_setUnreadChats(int count)
{
    if (count == _unreadChats)
        return;

    _unreadChats = count;
    emit unreadChatsChanged();
}
