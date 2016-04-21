#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>



class Entry: public QObject
{
    Q_OBJECT
public:
    Entry(const QJsonObject data, QObject* parent = nullptr);

    Q_INVOKABLE int         id()                const { return _id; }
    Q_INVOKABLE QDateTime   createdAt()         const { return _createdAt; }
    Q_INVOKABLE QString     url()               const { return _url; }
    Q_INVOKABLE QString     type()              const { return _type; }
    Q_INVOKABLE bool        isVotable()         const { return _isVotable; }
    Q_INVOKABLE bool        isPrivate()         const { return _isPrivate; }
    Q_INVOKABLE QJsonObject tlog()              const { return _tlog; }
    Q_INVOKABLE QJsonObject author()            const { return _author; }
    Q_INVOKABLE QJsonObject rating()            const { return _rating; }
    Q_INVOKABLE int         commentsCount()     const { return _commentsCount; }
    Q_INVOKABLE QString     title()             const { return _title; }
    Q_INVOKABLE QString     truncatedTitle()    const { return _truncatedTitle; }
    Q_INVOKABLE QString     text()              const { return _text; }
    Q_INVOKABLE QString     truncatedText()     const { return _truncatedText; }
    Q_INVOKABLE QJsonArray  imageAttach()       const { return _imageAttach; }
    Q_INVOKABLE QJsonObject imagePreview()      const { return _imagePreview; }

private:
    int         _id;
    QDateTime   _createdAt;
    QString     _url;
    QString     _type;
    bool        _isVotable;
    bool        _isPrivate;
    QJsonObject _tlog;
    QJsonObject _author;
    QJsonObject _rating;
    int         _commentsCount;
    QString     _title;
    QString     _truncatedTitle;
    QString     _text;
    QString     _truncatedText;
    QJsonArray  _imageAttach;
    QJsonObject _imagePreview;

    int _row;
};

#endif // DATASTRUCTURES_H
