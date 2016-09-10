#include "Tlog.h"

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"

#include "Author.h"



Tlog::Tlog(QObject* parent)
    : TastyData(parent)
    , _myRelation(Undefined)
    , _hisRelation(Undefined)
    , _author(new Author(this))
{

}



Tlog::Tlog(const QJsonObject data, QObject *parent)
    : TastyData(parent)
    , _author(nullptr)
{
    init(data);
}



void Tlog::setId(const int id)
{
    if (id <= 0 || id == _id)
        return;

    _id = id;
    emit idChanged();

    _slug.clear();

    reload();
}



void Tlog::setSlug(const QString slug)
{
    if (slug.isEmpty() || slug == _slug)
        return;

   _slug = slug;
   _id = 0;
   emit idChanged();

   reload();
}



void Tlog::init(const QJsonObject data)
{
    _id = data.value("id").toInt();
    _slug = data.value("slug").toString();
    _title = data.value("title").toString();
    _entriesCount = "<h1>" + Tasty::num2str(data.value("total_entries_count").toInt(),
                                            "</h1>запись", "</h1>записи", "</h1>записей");
    _publicEntriesCount = "<h1>" + Tasty::num2str(data.value("public_entries_count").toInt(),
                                                  "</h1>запись", "</h1>записи", "</h1>записей");
    _privateEntriesCount = "<h1>" + Tasty::num2str(data.value("private_entries_count").toInt(),
                                                   "</h1>скрыта", "</h1>скрыто", "</h1>скрыто");

    if (data.contains("stats"))
    {
        auto stats = data.value("stats").toObject();
        _favoritesCount = QString("<h1>%1</h1>в избранном")
                .arg(stats.value("favorites_count").toInt());
        _commentsCount = Tasty::num2str(stats.value("comments_count").toInt(),
                                        "комментарий", "комментария", "комментариев");
        _tagsCount = Tasty::num2str(stats.value("tags_count").toInt(),
                                    "тег", "тега", "тегов");
        _daysCount = Tasty::num2str(stats.value("days_count").toInt(),
                                    "день на Тейсти", "дня на Тейсти", "дней на Тейсти");
    }
    else
    {
        _favoritesCount = "Избранное";
        _commentsCount.clear();
        _tagsCount.clear();

        auto date = QDateTime::fromString(data.value("created_at").toString().left(19), "yyyy-MM-ddTHH:mm:ss");
        auto today = QDateTime::currentDateTime();
        int days = (today.toMSecsSinceEpoch() - date.toMSecsSinceEpoch()) / (24 * 60 * 60 * 1000);
        _daysCount = Tasty::num2str(days, "день на Тейсти", "дня на Тейсти", "дней на Тейсти");
    }

    auto relations = data.value("relationships_summary").toObject();
    _followersCount = "<h1>" + Tasty::num2str(relations.value("followers_count").toInt(),
                                              "</h1>подписчик", "</h1>подписчика", "</h1>подписчиков");
    _followingsCount = "<h1>" + Tasty::num2str(relations.value("followings_count").toInt(),
                                               "</h1>подписка", "</h1>подписки", "</h1>подписок");
    _ignoredCount = "<h1>" + Tasty::num2str(relations.value("ignored_count").toInt(),
                                            "</h1>блокирован", "</h1>блокировано", "</h1>блокировано");

    _hisRelation = _relationship(data, "his_relationship");
    _myRelation =  _relationship(data, "my_relationship");

    auto authorData = data.contains("author")
            ? data.value("author").toObject() : data;
    if (_author)
        _author->init(authorData);
    else
        _author = new Author(authorData, this);

    emit idChanged();
    emit updated();
}



void Tlog::reload()
{
    if ((_slug.isEmpty() && !_id) || isLoading())
            return;

    auto url = QString("v1/tlog/%1.json");
    if (_id)
        url = url.arg(_id);
    else
        url = url.arg(_slug);

    _request = new ApiRequest(url);
    connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(init(QJsonObject)));

    _initRequest();
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
