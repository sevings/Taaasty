#pragma once

#include <QObject>
#include <QJsonObject>

#include "MessageBase.h"

class Notification;



class Comment: public MessageBase
{
    Q_OBJECT

    friend class PusherClient;

    Q_PROPERTY(bool        isEditable   MEMBER _isEditable     NOTIFY updated)
    Q_PROPERTY(bool        isReportable MEMBER _isReportable   NOTIFY updated)
    Q_PROPERTY(bool        isDeletable  MEMBER _isDeletable    NOTIFY updated)

public:
    Comment(QObject* parent = nullptr);
    Comment(const QJsonObject data, QObject* parent = nullptr);
    ~Comment();

signals:
    void updated();

public slots:
    void edit(const QString text);
    void remove();

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
