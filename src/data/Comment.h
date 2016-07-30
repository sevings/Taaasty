#pragma once

#include <QObject>
#include <QJsonObject>

#include "messagebase.h"

class Notification;



class Comment: public MessageBase
{
    Q_OBJECT

    friend class CommentsModel;
    friend class PusherClient;

    Q_PROPERTY(bool        isEditable   MEMBER _isEditable     NOTIFY updated)
    Q_PROPERTY(bool        isReportable MEMBER _isReportable   NOTIFY updated)
    Q_PROPERTY(bool        isDeletable  MEMBER _isDeletable    NOTIFY updated)

public:
    Comment(QObject* parent = nullptr);
    Comment(const QJsonObject data, QObject* parent = nullptr);
    Comment(const Notification* data, QObject* parent = nullptr);
    ~Comment();

public slots:
    void edit(const QString text);
    void remove();

signals:
    void updated();

private slots:
    void _init(const QJsonObject data);
    void _update(const QJsonObject data);
    void _correctHtml();
    void _remove(const QString data);

private:
    bool        _isEditable;
    bool        _isReportable;
    bool        _isDeletable;
};
