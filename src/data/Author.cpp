#include "Author.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"



Author::Author(QObject* parent)
    : User(parent)
    , _isFemale(false)
    , _isPrivacy(false)
    , _isOnline(false)
    , _isFlow(false)
    , _isPremium(false)
    , _isDaylog(false)
{

}



Author::Author(const QJsonObject data, QObject *parent)
    : User(data, parent)
{
    _init(data);
}



bool Author::isFemale() const
{
    return _isFemale;
}



bool Author::isFlow() const
{
    return _isFlow;
}



bool Author::isPremium() const
{
    return _isPremium;
}



bool Author::isDaylog() const
{
    return _isDaylog;
}



void Author::checkStatus()
{
    auto url = QString("online_statuses.json?user_ids=%1").arg(id());
    auto request = new ApiRequest(url);

    Q_TEST(connect(request, SIGNAL(success(QJsonArray)), this, SLOT(_initStatus(QJsonArray))));
}



void Author::_init(const QJsonObject data)
{
    User::_init(data);

    _isFemale  = data.value("is_female").toBool();
    _isPrivacy = data.value("is_privacy").toBool();
    _isFlow    = data.value("is_flow").toBool();
    _isPremium = data.value("is_premium").toBool();
    _isDaylog  = data.value("is_daylog").toBool();
    _title     = data.value("title").toString();

    _entriesCount = Tasty::num2str(data.value("total_entries_count").toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value("public_entries_count").toInt(),
                                         "запись", "записи", "записей");
    _privateEntriesCount = Tasty::num2str(data.value("private_entries_count").toInt(),
                                          "скрытая запись", "скрытые записи", "скрытых записей");

    auto date = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    auto today = QDateTime::currentDateTime();
    int days = qRound(double(today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000));
    _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");

    _followingsCount = Tasty::num2str(data.value("followings_count").toInt(), "подписка", "подписки", "подписок");

    emit authorUpdated();

    _initStatus(data);
}



void Author::_initStatus(const QJsonArray data)
{
    if (data.size())
        _initStatus(data.first().toObject());
}



void Author::_initStatus(const QJsonObject data)
{
    _isOnline  = data.value("is_online").toBool();
    auto last = data.value("last_seen_at").toString();
    if (_isOnline)
        _lastSeenAt = "Онлайн";
    else if (last.isEmpty())
        _lastSeenAt = "Не в сети";
    else
        _lastSeenAt = QString("Был%1 в сети %2").arg(_isFemale ? "а" : "")
                .arg(Tasty::parseDate(last, false));

    emit statusUpdated();
}
