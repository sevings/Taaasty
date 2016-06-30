#include "Tlog.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"

#include "Author.h"



Tlog::Tlog(const QJsonObject data, QObject *parent)
    : QObject(parent)
    , _author(nullptr)
    , _loading(false)
{
    _init(data);
}



void Tlog::setId(const int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;

    auto request = new ApiRequest(QString("tlog/%1.json").arg(_id));
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));

    _loading = true;
    emit loadingChanged();
}



void Tlog::setSlug(const QString slug)
{
    if (slug.isEmpty() || slug == _slug)
        return;

   _slug = slug;

    auto request = new ApiRequest(QString("tlog/%1.json").arg(_slug));
    connect(request, SIGNAL(success(QJsonObject)), this, SLOT(_init(QJsonObject)));

    _loading = true;
    emit loadingChanged();
}



void Tlog::_init(const QJsonObject data)
{
    _id = data.value("id").toInt();
    _slug = data.value("slug").toString();
    _title = data.value("title").toString();
    _entriesCount = Tasty::num2str(data.value("total_entries_count").toInt(),
                                   "запись", "записи", "записей");
    _publicEntriesCount = Tasty::num2str(data.value("public_entries_count").toInt(),
                                         "запись", "записи", "записей");
    _privateEntriesCount = Tasty::num2str(data.value("private_entries_count").toInt(),
                                          "скрытая запись", "скрытые записи", "скрытых записей");

    auto date = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
    auto today = QDateTime::currentDateTime();
    int days = (today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000);
    _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");

    auto relations = data.value("relationships_summary").toObject();
    _followersCount = Tasty::num2str(relations.value("followers_count").toInt(), "подписчик", "подписчика", "подписчиков");
    _followingsCount = Tasty::num2str(relations.value("followings_count").toInt(), "подписка", "подписки", "подписок");
    _ignoredCount = Tasty::num2str(relations.value("ignored_count").toInt(), "блокирован", "блокировано", "блокировано");

    _hisRelation = _relationship(data, "his_relationship");
    _myRelation =  _relationship(data, "my_relationship");

    auto authorData = data.value("author").toObject();
    if (_author)
        _author->_init(authorData);
    else
        _author = new Author(authorData, this);

    emit updated();

    _loading = false;
    emit loadingChanged();
}



Tlog::Relationship Tlog::_relationship(const QJsonObject& data, const QString field)
{
    if (!data.contains(field))
        return Undefined;

    auto relation = data.value(field).toString();
    if (relation == "friend")
        return Friend;
    if (relation == "none")
        return None;
    if (relation == "ignored")
        return Ignored;
    if (relation.isEmpty())
        return Me;

    qDebug() << "relation:" << relation;
    return Undefined;
}
