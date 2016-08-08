#include "Comment.h"

#include "../defines.h"

#include "Notification.h"
#include "User.h"

#include "../tasty.h"
#include "../pusherclient.h"
#include "../apirequest.h"



Comment::Comment(QObject* parent)
    : MessageBase(parent)
    , _isEditable(false)
    , _isReportable(false)
    , _isDeletable(false)
{
    _user = new User(this);
}



Comment::Comment(const QJsonObject data, QObject *parent)
    : MessageBase(parent)
{
    _user = nullptr;

    _init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(htmlRecorrectionNeeded()), this, SLOT(_correctHtml())));
}



Comment::~Comment()
{
    Tasty::instance()->pusher()->removeComment(_id);
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

    delete _user;
    _user           = new User(data.value("user").toObject(), this);

    _text           = data.value("comment_html").toString();
    _createdAt      = Tasty::parseDate(data.value("created_at").toString());
    _isEditable     = data.value("can_edit").toBool();
    _isReportable   = data.value("can_report").toBool();
    _isDeletable    = data.value("can_delete").toBool();

    _correctHtml();

    emit baseUpdated();
    emit updated();

    Tasty::instance()->pusher()->addComment(this);
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
    Tasty::correctHtml(_text, false);

    emit textUpdated();
}



void Comment::_remove(const QString data)
{
    if (data == "true")
        deleteLater();
    else
        qDebug() << "removing commment:" << data;
}
