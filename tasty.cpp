#include "tasty.h"
#include "apirequest.h"

#include <QDateTime>
#include <QDebug>



Tasty::Tasty(QObject *parent)
    : QObject(parent)
    , _settings(new Settings(this))
    , _manager(new QNetworkAccessManager(this))
    , _busy(0)
{

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



QString Tasty::parseDate(const QString d)
{
    auto datetime = QDateTime::fromString(d.left(19), "yyyy-MM-ddTHH:mm:ss");
    auto date = datetime.date();
    auto today = QDate::currentDate();

    if (today == date)
        return datetime.toString("Сегодня в HH:mm");
    if (today == date.addDays(1))
        return datetime.toString("Вчера в HH:mm");

    bool showYear = date.year() != today.year();
    QString format = showYear ? "d MMM yyyy" : "d MMM в HH:mm";
    return datetime.toString(format);
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



void Tasty::_readComment(const QJsonObject data)
{
    Q_UNUSED(data);
}



void Tasty::_readMe(const QJsonObject data)
{
    qDebug() << data.value("title").toString();
}
