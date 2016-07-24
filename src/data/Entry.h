#pragma once

#include <QObject>
#include <QJsonObject>

class Author;
class Tlog;
class Rating;
class Media;
class Conversation;
//class CommentsModel;
class AttachedImagesModel;
class MessagesModel;



class EntryBase: public QObject
{
    Q_OBJECT

public:
    EntryBase(QObject* parent = nullptr);

    void load(int id);

    Author* author() const;
    QString text() const;
    QString title() const;
    QString type() const;

public slots:
    int entryId() const { return _id; }

signals:
    void loaded();
    void loadingError();

protected slots:
    void _initBase(QJsonObject data);
    void _maybeError();

protected:
    int         _id;
    Author*     _author;
    QString     _text;
    QString     _title;
    QString     _type;
};



class Entry: public EntryBase
{
    Q_OBJECT

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
    Q_PROPERTY(QString     title          MEMBER _title             NOTIFY htmlUpdated)
    Q_PROPERTY(QString     truncatedTitle MEMBER _truncatedTitle    NOTIFY updated)
    Q_PROPERTY(QString     text           MEMBER _text              NOTIFY htmlUpdated)
    Q_PROPERTY(QString     truncatedText  MEMBER _truncatedText     NOTIFY updated)
    Q_PROPERTY(QString     source         MEMBER _source            NOTIFY updated)
    Q_PROPERTY(Media*      media          MEMBER _media             NOTIFY updated)
//    Q_PROPERTY(QJsonObject imagePreview   MEMBER _imagePreview      NOTIFY updated)
    Q_PROPERTY(int         wordCount      MEMBER _wordCount         NOTIFY updated)

    Q_PROPERTY(Conversation* chat         MEMBER _chat              NOTIFY updated)
    Q_PROPERTY(AttachedImagesModel* attachedImagesModel READ attachedImagesModel NOTIFY updated)

    Q_PROPERTY(bool loading               MEMBER _loading           NOTIFY loadingChanged)

public:
    Entry(QObject* parent = nullptr);
    Entry(const QJsonObject data, Conversation* chat);
    Entry(const QJsonObject data, QObject* parent = nullptr);

//    Q_INVOKABLE CommentsModel*       commentsModel()       { return _commentsModel; }
    Q_INVOKABLE AttachedImagesModel* attachedImagesModel() { return _attachedImagesModel; }

    int wordCount() const;

    Rating* rating() const;

    bool loading() const;

    Tlog* tlog() const;

    int commentsCount() const;

public slots:
    void setId(const int id);
    void reload();

    void addComment(const QString text);
    void watch();
    void favorite();

signals:
    void updated();
    void updatingError();
    void htmlUpdated();

    void commentsCountChanged();
    void watchedChanged();
    void favoritedChanged();
    void commentAdded(const QJsonObject data);

    void loadingChanged();

private slots:
    void _init(const QJsonObject data);
    void _changeWatched(const QJsonObject data);
    void _changeFavorited(const QJsonObject data);
    void _setCommentsCount(int tc);
    void _setWatched();
    void _correctHtml();
    void _setNotLoading();

private:
    QString     _createdAt;
    QString     _url;
    bool        _isVotable;
    bool        _isWatchable;
    bool        _isWatched;
    bool        _isFavoritable;
    bool        _isFavorited;
    bool        _isPrivate;
    Tlog*       _tlog;
    Rating*     _rating;
    int         _commentsCount;
    QString     _truncatedTitle;
    QString     _truncatedText;
    QString     _source;
    Media*      _media;
//    QJsonObject _imagePreview;
    int         _wordCount;

//    CommentsModel*       _commentsModel;
    Conversation*        _chat;
    AttachedImagesModel* _attachedImagesModel;

    bool        _loading;
};
