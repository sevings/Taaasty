#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

class Entry;
class Comment;
class User;
class Rating;

class CommentsModel;



class Entry: public QObject
{
    Q_OBJECT

    friend class CommentsModel;

    Q_PROPERTY(int         id             MEMBER _id                CONSTANT)
    Q_PROPERTY(QDateTime   createdAt      MEMBER _createdAt         CONSTANT)
    Q_PROPERTY(QString     url            MEMBER _url               CONSTANT)
    Q_PROPERTY(QString     type           MEMBER _type              CONSTANT)
    Q_PROPERTY(bool        isVotable      MEMBER _isVotable         CONSTANT)
    Q_PROPERTY(bool        isPrivate      MEMBER _isPrivate         CONSTANT)
    Q_PROPERTY(QJsonObject tlog           MEMBER _tlog              CONSTANT)
    Q_PROPERTY(QJsonObject author         MEMBER _author            CONSTANT)
    Q_PROPERTY(Rating*     rating         MEMBER _rating            CONSTANT)
    Q_PROPERTY(int         commentsCount  MEMBER _commentsCount     NOTIFY commentsCountChanged)
    Q_PROPERTY(QString     title          MEMBER _title             CONSTANT)
    Q_PROPERTY(QString     truncatedTitle MEMBER _truncatedTitle    CONSTANT)
    Q_PROPERTY(QString     text           MEMBER _text              CONSTANT)
    Q_PROPERTY(QString     truncatedText  MEMBER _truncatedText     CONSTANT)
    Q_PROPERTY(QJsonArray  imageAttach    MEMBER _imageAttach       CONSTANT)
    Q_PROPERTY(QJsonObject imagePreview   MEMBER _imagePreview      CONSTANT)

public:
    Entry(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    Q_INVOKABLE CommentsModel* commentsModel();

public slots:
    void addComment(const QString text);

signals:
    void commentsCountChanged();
    void commentAdded(const QJsonObject data);

private slots:
    void _addComment();

private:
    int         _id;
    QDateTime   _createdAt;
    QString     _url;
    QString     _type;
    bool        _isVotable;
    bool        _isPrivate;
    QJsonObject _tlog;
    QJsonObject _author;
    Rating*     _rating;
    int         _commentsCount;
    QString     _title;
    QString     _truncatedTitle;
    QString     _text;
    QString     _truncatedText;
    QJsonArray  _imageAttach;
    QJsonObject _imagePreview;

    CommentsModel* _commentsModel;
};



class Comment: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int         id           MEMBER _id             CONSTANT)
    Q_PROPERTY(User*       user         MEMBER _user           CONSTANT)
    Q_PROPERTY(QString     html         MEMBER _html           CONSTANT)
    Q_PROPERTY(QDateTime   createdAt    MEMBER _createdAt      CONSTANT)
    Q_PROPERTY(bool        isEditable   MEMBER _isEditable     CONSTANT)
    Q_PROPERTY(bool        isReportable MEMBER _isReportable   CONSTANT)
    Q_PROPERTY(bool        isDeletable  MEMBER _isDeletable    CONSTANT)

public:
    Comment(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

public slots:
    void edit(const QString text);
    void remove();

signals:
    void htmlChanged();

private slots:
    void _init(const QJsonObject data);

private:
    int         _id;
    User*       _user;
    QString     _html;
    QDateTime   _createdAt;
    bool        _isEditable;
    bool        _isReportable;
    bool        _isDeletable;
};



class User: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int     id           MEMBER _id          CONSTANT)
    Q_PROPERTY(QString tlogUrl      MEMBER _tlogUrl     CONSTANT)
    Q_PROPERTY(QString name         MEMBER _name        CONSTANT)
    Q_PROPERTY(QString slug         MEMBER _slug        CONSTANT)

    Q_PROPERTY(QString originalPic  MEMBER _originalPic CONSTANT)
    Q_PROPERTY(QString largePic     MEMBER _largePic    CONSTANT)
    Q_PROPERTY(QString thumb128     MEMBER _thumb128    CONSTANT)
    Q_PROPERTY(QString thumb64      MEMBER _thumb64     CONSTANT)
    Q_PROPERTY(QString symbol       MEMBER _symbol      CONSTANT)

public:
    User(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

private:
    int     _id;
    QString _tlogUrl;
    QString _name;
    QString _slug;

    QString _originalPic;
    QString _largePic;
    QString _thumb128;
    QString _thumb64;
    QString _symbol;
};



class Rating: public QObject
{
    Q_OBJECT

    Q_PROPERTY (int  entryId    MEMBER _entryId    NOTIFY dataChanged)
    Q_PROPERTY (int  votes      MEMBER _votes      NOTIFY dataChanged)
    Q_PROPERTY (int  rating     MEMBER _rating     NOTIFY dataChanged)
    Q_PROPERTY (bool isVoted    MEMBER _isVoted    NOTIFY dataChanged)
    Q_PROPERTY (bool isVotable  MEMBER _isVotable  NOTIFY dataChanged)

public:
    Rating(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

public slots:
    void vote();

signals:
    void dataChanged();

private slots:
    void _init(const QJsonObject data);

private:
    int  _entryId;
    int  _votes;
    int  _rating;
    bool _isVoted;
    bool _isVotable;
};

//Q_DECLARE_METATYPE(Entry)


#endif // DATASTRUCTURES_H
