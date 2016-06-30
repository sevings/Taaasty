#pragma once

#include <QObject>
#include <QJsonObject>

class User;
class Notification;



class Comment: public QObject
{
    Q_OBJECT

    friend class CommentsModel;

    Q_PROPERTY(int         id           MEMBER _id             NOTIFY updated)
    Q_PROPERTY(User*       user         MEMBER _user           NOTIFY updated)
    Q_PROPERTY(QString     html         MEMBER _html           NOTIFY htmlUpdated)
    Q_PROPERTY(QString     createdAt    MEMBER _createdAt      NOTIFY updated)
    Q_PROPERTY(bool        isEditable   MEMBER _isEditable     NOTIFY updated)
    Q_PROPERTY(bool        isReportable MEMBER _isReportable   NOTIFY updated)
    Q_PROPERTY(bool        isDeletable  MEMBER _isDeletable    NOTIFY updated)

public:
    Comment(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);
    Comment(const Notification* data, QObject* parent = nullptr);

public slots:
    void edit(const QString text);
    void remove();

signals:
    void updated();
    void htmlUpdated();

private slots:
    void _init(const QJsonObject data);
    void _update(const QJsonObject data);
    void _correctHtml();
    void _remove(const QString data);

private:
    int         _id;
    User*       _user;
    QString     _html;
    QString     _createdAt;
    bool        _isEditable;
    bool        _isReportable;
    bool        _isDeletable;
};
