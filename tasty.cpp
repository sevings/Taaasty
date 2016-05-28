#include "tasty.h"
#include "apirequest.h"

#include "defines.h"

#include <QDateTime>
#include <QRegularExpression>
#include <QDebug>



Tasty::Tasty(QObject *parent)
    : QObject(parent)
    , _settings(new Settings(this))
    , _manager(new QNetworkAccessManager(this))
    , _busy(0)
    , _entryImageWidth(_settings->maxImageWidth())
    , _commentImageWidth(_entryImageWidth)
{
    Q_TEST(connect(_manager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
                   this, SLOT(_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility))));
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
    QRegularExpression slugRe("([^'/>\\w\\-\\.])(~|@)([\\w\\-\\.]+)");
    html.replace(slugRe, "\\1<a href='http://taaasty.com/~\\3'>\\2\\3</a>");

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



void Tasty::authorize(const QString login, const QString password)
{
    qDebug() << "authorize";

    auto data = QString("email=%1&password=%2")
            .arg(login)
            .arg(password);

    auto request = new ApiRequest("sessions.json", false,
                                  QNetworkAccessManager::PostOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));
}



void Tasty::postEntry(const QString title, const QString content)
{
    auto data = QString("title=%1&text=%2&privacy=%3&tlog_id=%4")
            .arg(title)
            .arg(content)
            .arg("public_with_voting")
            .arg("");

    qDebug() << data;

    auto request = new ApiRequest("entries/text.json", true,
                                  QNetworkAccessManager::PostOperation, data);

    Q_UNUSED(request);

    //connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readAccessToken(const QJsonObject)));
}



void Tasty::getMe()
{
    auto request = new ApiRequest("users/me.json", true);
    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_readMe(const QJsonObject)));
}



void Tasty::_readAccessToken(const QJsonObject data)
{
    auto apiKey = data.value("api_key").toObject();
    auto accessToken = apiKey.value("access_token").toString();
    auto expiresAt = apiKey.value("expires_at").toString();

    qDebug() << "Access token:" << accessToken;
    qDebug() << "Expires at: " << expiresAt;

    _settings->setAccessToken(accessToken);
    _settings->setExpiresAt(expiresAt);

    emit authorized();
}



void Tasty::_showNetAccessibility(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (accessible == QNetworkAccessManager::NotAccessible)
        emit error(0, "Сеть недоступна");
}



void Tasty::_readMe(const QJsonObject data)
{
    qDebug() << data.value("title").toString();
}
