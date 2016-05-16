#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class CalendarEntry;
class Entry;
class Comment;
class User;
class Author;
class Tlog;
class Rating;
class AttachedImage;
class Notification;

class CommentsModel;
class AttachedImagesModel;



class CalendarEntry: public QObject
{
    Q_OBJECT

    friend class CalendarModel;

    Q_PROPERTY(int     id             MEMBER _id             CONSTANT)
    Q_PROPERTY(QString createdAt      MEMBER _createdAt      CONSTANT)
    Q_PROPERTY(QString url            MEMBER _url            CONSTANT)
    Q_PROPERTY(bool    isPrivate      MEMBER _isPrivate      CONSTANT)
    Q_PROPERTY(QString type           MEMBER _type           CONSTANT)
    Q_PROPERTY(int     commentsCount  MEMBER _commentsCount  CONSTANT)
    Q_PROPERTY(QString truncatedTitle MEMBER _truncatedTitle CONSTANT)
    Q_PROPERTY(bool    isFlow         MEMBER _isFlow         CONSTANT)

public:
    CalendarEntry(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    Entry* full();

private:
    int     _id;
    QString _createdAt;
    QString _url;
    bool    _isPrivate;
    QString _type;
    int     _commentsCount;
    QString _truncatedTitle;
    bool    _isFlow;

    Entry* _entry;
};



class Entry: public QObject
{
    Q_OBJECT

    friend class CommentsModel;
    friend class Bayes;

    Q_PROPERTY(int         entryId        READ entryId WRITE setId  NOTIFY updated)
    Q_PROPERTY(QString     createdAt      MEMBER _createdAt         NOTIFY updated)
    Q_PROPERTY(QString     url            MEMBER _url               NOTIFY updated)
    Q_PROPERTY(QString     type           MEMBER _type              NOTIFY updated)
    Q_PROPERTY(bool        isVotable      MEMBER _isVotable         NOTIFY updated)
    Q_PROPERTY(bool        isWatchable    MEMBER _isWatchable       NOTIFY updated)
    Q_PROPERTY(bool        isWatched      MEMBER _isWatched         NOTIFY watchedChanged)
    Q_PROPERTY(bool        isFavoritable  MEMBER _isFavoritable     NOTIFY updated)
    Q_PROPERTY(bool        isFavorited    MEMBER _isFavorited       NOTIFY favoritedChanged)
    Q_PROPERTY(bool        isPrivate      MEMBER _isPrivate         NOTIFY updated)
    Q_PROPERTY(Tlog*       tlog           MEMBER _tlog              NOTIFY updated)
    Q_PROPERTY(Author*     author         MEMBER _author            NOTIFY updated)
    Q_PROPERTY(Rating*     rating         MEMBER _rating            NOTIFY updated)
    Q_PROPERTY(int         commentsCount  MEMBER _commentsCount     NOTIFY commentsCountChanged)
    Q_PROPERTY(QString     title          MEMBER _title             NOTIFY updated)
    Q_PROPERTY(QString     truncatedTitle MEMBER _truncatedTitle    NOTIFY updated)
    Q_PROPERTY(QString     text           MEMBER _text              NOTIFY updated)
    Q_PROPERTY(QString     truncatedText  MEMBER _truncatedText     NOTIFY updated)
    Q_PROPERTY(QString     source         MEMBER _source            NOTIFY updated)
    Q_PROPERTY(QJsonObject imagePreview   MEMBER _imagePreview      NOTIFY updated)
    Q_PROPERTY(int         wordCount      MEMBER _wordCount         NOTIFY updated)

    Q_PROPERTY(CommentsModel*       commentsModel       READ commentsModel       NOTIFY updated)
    Q_PROPERTY(AttachedImagesModel* attachedImagesModel READ attachedImagesModel NOTIFY updated)

    Q_PROPERTY(bool loading               MEMBER _loading           NOTIFY loadingChanged)

public:
    Entry(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    Q_INVOKABLE CommentsModel*       commentsModel()       { return _commentsModel; }
    Q_INVOKABLE AttachedImagesModel* attachedImagesModel() { return _attachedImagesModel; }

public slots:
    int entryId() const { return _id; }
    void setId(const int id);

    void addComment(const QString text);
    void watch();
    void favorite();

signals:
    void updated();

    void commentsCountChanged();
    void watchedChanged();
    void favoritedChanged();
    void commentAdded(const QJsonObject data);

    void loadingChanged();

private slots:
    void _init(const QJsonObject data);
    void _addComment();
    void _changeWatched(const QJsonObject data);
    void _changeFavorited(const QJsonObject data);

private:
    int         _id;
    QString     _createdAt;
    QString     _url;
    QString     _type;
    bool        _isVotable;
    bool        _isWatchable;
    bool        _isWatched;
    bool        _isFavoritable;
    bool        _isFavorited;
    bool        _isPrivate;
    Tlog*       _tlog;
    Author*     _author;
    Rating*     _rating;
    int         _commentsCount;
    QString     _title;
    QString     _truncatedTitle;
    QString     _text;
    QString     _truncatedText;
    QString     _source;
    QJsonObject _imagePreview;
    int         _wordCount;

    CommentsModel*       _commentsModel;
    AttachedImagesModel* _attachedImagesModel;

    bool        _loading;
};



class AttachedImage: public QObject
{
    Q_OBJECT

    friend class AttachedImagesModel;

    Q_PROPERTY(QString url    MEMBER _url    CONSTANT)
    Q_PROPERTY(int     height MEMBER _height CONSTANT)
    Q_PROPERTY(int     width  MEMBER _width  CONSTANT)
    Q_PROPERTY(QString type   MEMBER _type   CONSTANT)

public:
    AttachedImage(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

private:
    QString _url;
    QString _type;
    int     _width;
    int     _height;
};



class Comment: public QObject
{
    Q_OBJECT

    friend class CommentsModel;

    Q_PROPERTY(int         id           MEMBER _id             NOTIFY updated)
    Q_PROPERTY(User*       user         MEMBER _user           NOTIFY updated)
    Q_PROPERTY(QString     html         MEMBER _html           NOTIFY updated)
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

private slots:
    void _init(const QJsonObject data);
    void _update(const QJsonObject data);

private:
    int         _id;
    User*       _user;
    QString     _html;
    QString     _createdAt;
    bool        _isEditable;
    bool        _isReportable;
    bool        _isDeletable;
};



class User: public QObject
{
    Q_OBJECT

    friend class Bayes;
    friend class Trainer;

    Q_PROPERTY(int     id           MEMBER _id          CONSTANT)
    Q_PROPERTY(QString tlogUrl      MEMBER _tlogUrl     CONSTANT)
    Q_PROPERTY(QString name         MEMBER _name        CONSTANT)
    Q_PROPERTY(QString slug         MEMBER _slug        CONSTANT)

    Q_PROPERTY(QString originalPic     MEMBER _originalPic     CONSTANT)
    Q_PROPERTY(QString largePic        MEMBER _largePic        CONSTANT)
    Q_PROPERTY(QString thumb128        MEMBER _thumb128        CONSTANT)
    Q_PROPERTY(QString thumb64         MEMBER _thumb64         CONSTANT)
    Q_PROPERTY(QString symbol          MEMBER _symbol          CONSTANT)
    Q_PROPERTY(QString backgroundColor MEMBER _backgroundColor CONSTANT)
    Q_PROPERTY(QString nameColor       MEMBER _nameColor       CONSTANT)

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
    QString _backgroundColor;
    QString _nameColor;
};



class Author: public User
{
    Q_OBJECT

    friend class Bayes;
    friend class Trainer;

    Q_PROPERTY(bool    isFemale            MEMBER _isFemale            CONSTANT)
    Q_PROPERTY(bool    isPrivacy           MEMBER _isPrivacy           CONSTANT)
    Q_PROPERTY(bool    isOnline            MEMBER _isOnline            CONSTANT)
    Q_PROPERTY(bool    isFlow              MEMBER _isFlow              CONSTANT)
    Q_PROPERTY(bool    isPremium           MEMBER _isPremium           CONSTANT)
    Q_PROPERTY(bool    isDaylog            MEMBER _isDaylog            CONSTANT)
    Q_PROPERTY(QString title               MEMBER _title               CONSTANT)
    Q_PROPERTY(QString entriesCount        MEMBER _entriesCount        CONSTANT)
    Q_PROPERTY(QString publicEntriesCount  MEMBER _publicEntriesCount  CONSTANT)
    Q_PROPERTY(QString privateEntriesCount MEMBER _privateEntriesCount CONSTANT)
    Q_PROPERTY(QString daysCount           MEMBER _daysCount           CONSTANT)
    Q_PROPERTY(QString followingsCount     MEMBER _followingsCount     CONSTANT)

public:
    Author(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

private:
    bool    _isFemale;
    bool    _isPrivacy;
    bool    _isOnline;
    bool    _isFlow;
    bool    _isPremium;
    bool    _isDaylog;
    QString _title;
    QString _entriesCount;
    QString _publicEntriesCount;
    QString _privateEntriesCount;
    QString _daysCount;
    QString _followingsCount;
};



class Tlog: public QObject
{
    Q_OBJECT

    friend class Trainer;

    Q_PROPERTY(int     tlogId              READ tlogId   WRITE setId   NOTIFY updated)
    Q_PROPERTY(QString title               MEMBER _title               NOTIFY updated)
    Q_PROPERTY(QString entriesCount        MEMBER _entriesCount        NOTIFY updated)
    Q_PROPERTY(QString publicEntriesCount  MEMBER _publicEntriesCount  NOTIFY updated)
    Q_PROPERTY(QString privateEntriesCount MEMBER _privateEntriesCount NOTIFY updated)
    Q_PROPERTY(QString daysCount           MEMBER _daysCount           NOTIFY updated)
    Q_PROPERTY(QString followersCount      MEMBER _followersCount      NOTIFY updated)
    Q_PROPERTY(QString followingsCount     MEMBER _followingsCount     NOTIFY updated)
    Q_PROPERTY(QString ignoredCount        MEMBER _ignoredCount        NOTIFY updated)
    Q_PROPERTY(bool    isFollowingMe       MEMBER _isFollowingMe       NOTIFY updated)
    Q_PROPERTY(bool    amIFollowing        MEMBER _amIFollowing        NOTIFY updated)
    Q_PROPERTY(Author* author              MEMBER _author              NOTIFY updated)

    Q_PROPERTY(bool loading                MEMBER _loading             NOTIFY loadingChanged)

public:
    Tlog(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    Author* author() const { return _author; }

public slots:
    int tlogId() const { return _id; }
    void setId(const int id);

signals:
    void updated();
    void loadingChanged();

private slots:
    void _init(const QJsonObject data);

private:
    int     _id;
    QString _title;
    QString _entriesCount;
    QString _publicEntriesCount;
    QString _privateEntriesCount;
    QString _daysCount;
    QString _followersCount;
    QString _followingsCount;
    QString _ignoredCount;
    bool    _isFollowingMe;
    bool    _amIFollowing;
    Author* _author;

    bool    _loading;
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



class Notification: public QObject
{
    Q_OBJECT
    
    friend class NotificationsModel;
    friend class Comment;
    
    Q_PROPERTY(int     id         MEMBER _id         CONSTANT)
    Q_PROPERTY(QString createdAt  MEMBER _createdAt  CONSTANT)
    Q_PROPERTY(User*   sender     MEMBER _sender     CONSTANT)
    Q_PROPERTY(bool    read       MEMBER _read       NOTIFY read)
    Q_PROPERTY(QString action     MEMBER _action     CONSTANT)
    Q_PROPERTY(QString actionText MEMBER _actionText CONSTANT)
    Q_PROPERTY(QString text       MEMBER _text       CONSTANT)
    Q_PROPERTY(int     entityId   MEMBER _entityId   CONSTANT)
    Q_PROPERTY(QString entityType MEMBER _entityType CONSTANT)
    Q_PROPERTY(int     parentId   MEMBER _parentId   CONSTANT)
    Q_PROPERTY(QString parentType MEMBER _parentType CONSTANT)
    
public:
    Notification(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);
    
signals:
    void read();
    
private:
    int     _id;
    QString _createdAt;
    User*   _sender;
    bool    _read;
    QString _action;
    QString _actionText;
    QString _text;
    int     _entityId;
    QString _entityType;
    int     _parentId;
    QString _parentType;
    //QString _image;
};

#endif // DATASTRUCTURES_H
