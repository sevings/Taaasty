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



class Entry: public QObject
{
    Q_OBJECT

    friend class FeedModel;

    Q_PROPERTY(int         id               NOTIFY dataChanged READ id)
    Q_PROPERTY(QDateTime   createdAt        NOTIFY dataChanged READ createdAt)
    Q_PROPERTY(QString     url              NOTIFY dataChanged READ url)
    Q_PROPERTY(QString     type             NOTIFY dataChanged READ type)
    Q_PROPERTY(bool        isVotable        NOTIFY dataChanged READ isVotable)
    Q_PROPERTY(bool        isPrivate        NOTIFY dataChanged READ isPrivate)
    Q_PROPERTY(QJsonObject tlog             NOTIFY dataChanged READ tlog)
    Q_PROPERTY(QJsonObject author           NOTIFY dataChanged READ author)
    Q_PROPERTY(Rating*     rating           NOTIFY dataChanged READ rating)
    Q_PROPERTY(int         commentsCount    NOTIFY dataChanged READ commentsCount)
    Q_PROPERTY(QString     title            NOTIFY dataChanged READ title)
    Q_PROPERTY(QString     truncatedTitle   NOTIFY dataChanged READ truncatedTitle)
    Q_PROPERTY(QString     text             NOTIFY dataChanged READ text)
    Q_PROPERTY(QString     truncatedText    NOTIFY dataChanged READ truncatedText)
    Q_PROPERTY(QJsonArray  imageAttach      NOTIFY dataChanged READ imageAttach)
    Q_PROPERTY(QJsonObject imagePreview     NOTIFY dataChanged READ imagePreview)

public:
    Entry(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    Q_INVOKABLE int         id()                const { return _id; }
    Q_INVOKABLE QDateTime   createdAt()         const { return _createdAt; }
    Q_INVOKABLE QString     url()               const { return _url; }
    Q_INVOKABLE QString     type()              const { return _type; }
    Q_INVOKABLE bool        isVotable()         const { return _isVotable; }
    Q_INVOKABLE bool        isPrivate()         const { return _isPrivate; }
    Q_INVOKABLE QJsonObject tlog()              const { return _tlog; }
    Q_INVOKABLE QJsonObject author()            const { return _author; }
    Q_INVOKABLE Rating*     rating()            const { return _rating; }
    Q_INVOKABLE int         commentsCount()     const { return _commentsCount; }
    Q_INVOKABLE QString     title()             const { return _title; }
    Q_INVOKABLE QString     truncatedTitle()    const { return _truncatedTitle; }
    Q_INVOKABLE QString     text()              const { return _text; }
    Q_INVOKABLE QString     truncatedText()     const { return _truncatedText; }
    Q_INVOKABLE QJsonArray  imageAttach()       const { return _imageAttach; }
    Q_INVOKABLE QJsonObject imagePreview()      const { return _imagePreview; }

signals:
    void dataChanged();

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

    int _row;
};



class Comment: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int         id           READ id)
    Q_PROPERTY(User*       user         READ user)
    Q_PROPERTY(QString     html         READ html)
    Q_PROPERTY(QDateTime   createdAt    READ createdAt)
    Q_PROPERTY(bool        isEditable   READ isEditable)
    Q_PROPERTY(bool        isReportable READ isReportable)
    Q_PROPERTY(bool        isDeletable  READ isDeletable)

public:
    Comment(const QJsonObject& data, QObject* parent = nullptr);

    Q_INVOKABLE int         id()            const { return _id; }
    Q_INVOKABLE User*       user()          const { return _user; }
    Q_INVOKABLE QString     html()          const { return _html; }
    Q_INVOKABLE QDateTime   createdAt()     const { return _createdAt; }
    Q_INVOKABLE bool        isEditable()    const { return _isEditable; }
    Q_INVOKABLE bool        isReportable()  const { return _isReportable; }
    Q_INVOKABLE bool        isDeletable()   const { return _isDeletable; }

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

    Q_PROPERTY(int         id       READ id)
    Q_PROPERTY(QString     tlogUrl  READ tlogUrl)
    Q_PROPERTY(QString     name     READ name)
    Q_PROPERTY(QString     slug     READ slug)
//    Q_PROPERTY(QJsonObject userpic  READ userpic)

    Q_PROPERTY(QString originalPic  READ originalPic)
    Q_PROPERTY(QString largePic     READ largePic)
    Q_PROPERTY(QString thumb128     READ thumb128)
    Q_PROPERTY(QString thumb64      READ thumb64)
    Q_PROPERTY(QString symbol       READ symbol)

public:
    User(const QJsonObject& data, QObject* parent = nullptr);

    Q_INVOKABLE int         id()        const { return _id; }
    Q_INVOKABLE QString     tlogUrl()   const { return _tlogUrl; }
    Q_INVOKABLE QString     name()      const { return _name; }
    Q_INVOKABLE QString     slug()      const { return _slug; }
//    Q_INVOKABLE QJsonObject userpic()   const { return _userpic; }

    Q_INVOKABLE QString originalPic()   const { return _originalPic; }
    Q_INVOKABLE QString largePic()      const { return _largePic; }
    Q_INVOKABLE QString thumb128()      const { return _thumb128; }
    Q_INVOKABLE QString thumb64()       const { return _thumb64; }
    Q_INVOKABLE QString symbol()        const { return _symbol; }

private:
    int         _id;
    QString     _tlogUrl;
    QString     _name;
    QString     _slug;
//    QJsonObject _userpic;

    QString _originalPic;
    QString _largePic;
    QString _thumb128;
    QString _thumb64;
    QString _symbol;
};



class Rating: public QObject
{
    Q_OBJECT

    Q_PROPERTY (int  entryId    READ entryId    NOTIFY dataChanged)
    Q_PROPERTY (int  votes      READ votes      NOTIFY dataChanged)
    Q_PROPERTY (int  rating     READ rating     NOTIFY dataChanged)
    Q_PROPERTY (bool isVoted    READ isVoted    NOTIFY dataChanged)
    Q_PROPERTY (bool isVotable  READ isVotable  NOTIFY dataChanged)

public:
    Rating(const QJsonObject data = QJsonObject(), QObject* parent = nullptr);

    Q_INVOKABLE int  entryId()   const { return _entryId; }
    Q_INVOKABLE int  votes()     const { return _votes; }
    Q_INVOKABLE int  rating()    const { return _rating; }
    Q_INVOKABLE bool isVoted ()  const { return _isVoted; }
    Q_INVOKABLE bool isVotable() const { return _isVotable; }

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
