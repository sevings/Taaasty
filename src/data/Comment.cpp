#include "Comment.h"

#include "../defines.h"

#include "Notification.h"
#include "User.h"

#include "../tasty.h"
#include "../apirequest.h"



Comment::Comment(QObject* parent)
    : QObject(parent)
    , _id(0)
    , _user(new User(this))
    , _isEditable(false)
    , _isReportable(false)
    , _isDeletable(false)
{

}



Comment::Comment(const QJsonObject data, QObject *parent)
    : QObject(parent)
{
    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Comment::Comment(const Notification* data, QObject* parent)
    : QObject(parent)
{
    _id             = data->_entityId;
    _user           = data->_sender;
    _html           = data->_text;
    _createdAt      = Tasty::parseDate(data->_createdAt);
    _isEditable     = false;
    _isReportable   = false;
    _isDeletable    = false;

    auto entryId = data->_parentId;
    auto url = QString("v1/comments.json?entry_id=%1&from_comment_id=%2&limit=1").arg(entryId).arg(_id - 1);
    auto request = new ApiRequest(url);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_update(const QJsonObject))));
}



void Comment::edit(const QString text)
{
    auto url = QString("v1/comments/%1.json").arg(_id);
    auto data = QString("text=%1").arg(text);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::PutOperation, data);

    connect(request, SIGNAL(success(const QJsonObject)), this, SLOT(_init(const QJsonObject)));
}



void Comment::remove()
{
    auto url = QString("v1/comments/%1.json").arg(_id);
    auto request = new ApiRequest(url, true, QNetworkAccessManager::DeleteOperation);

    connect(request, SIGNAL(success(const QString)), this, SLOT(_remove(const QString)));
}



void Comment::_init(const QJsonObject data)
{
    _id             = data.value("id").toInt();
    _user           = new User(data.value("user").toObject(), this);
    _html           = data.value("comment_html").toString();
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
    _isEditable     = data.value("can_edit").toBool();
    _isReportable   = data.value("can_report").toBool();
    _isDeletable    = data.value("can_delete").toBool();

    _correctHtml();

    emit updated();
}



void Comment::_update(const QJsonObject data)
{
    auto list = data.value("comments").toArray();
    if (list.isEmpty())
        return;

    _init(list.first().toObject());
}



void Comment::_correctHtml()
{
    Tasty::correctHtml(_html, false);

    emit htmlUpdated();
}



void Comment::_remove(const QString data)
{
    if (data == "true")
        deleteLater();
    else
        qDebug() << "removing commment:" << data;
}
