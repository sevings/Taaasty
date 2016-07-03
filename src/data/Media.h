#pragma once

#include <QObject>
#include <QJsonObject>

class AttachedImage;



class Media: public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString        url       MEMBER _url       CONSTANT)
    Q_PROPERTY(QString        title     MEMBER _title     CONSTANT)
    Q_PROPERTY(int            duration  MEMBER _duration  CONSTANT)
    Q_PROPERTY(AttachedImage* thumbnail MEMBER _thumbnail CONSTANT)
    Q_PROPERTY(AttachedImage* icon      MEMBER _icon      CONSTANT)

public:
    Media(QObject* parent = nullptr);
    Media(const QJsonObject data, QObject* parent = nullptr);

private:
    QString _url;
    QString _title;
    int     _duration;

    AttachedImage* _thumbnail;
    AttachedImage* _icon;

};
