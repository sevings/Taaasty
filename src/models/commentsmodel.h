#ifndef COMMENTSMODEL_H
#define COMMENTSMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

class ApiRequest;
class Comment;
class Notification;
class Entry;



class CommentsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int entryId  READ entryId WRITE setEntryId)
    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit CommentsModel(Entry* entry = nullptr);

    void init(const QJsonArray feed, int totalCount);

    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int entryId() const { return _entryId; }
    Q_INVOKABLE void setEntryId(const int id);

    Q_INVOKABLE bool hasMore() const { return _comments.size() < _totalCount; }
    Q_INVOKABLE bool loading() const { return _loading; }

    Q_INVOKABLE void check();

    Comment* lastComment() const;

signals:
    void hasMoreChanged();
    void loadingChanged();
    void totalCountChanged(int tc);
    void lastCommentChanged();

public slots:
    void loadMore();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addComments(const QJsonObject data);
    void _addComments(const QJsonArray feed);
    void _addLastComments(const QJsonObject data);
    void _addComment(const QJsonObject data);
    void _removeComment(QObject* cmt);
    void _setNotLoading(QObject* request);

private:
    void            _setTotalCount(int tc);
    QList<Comment*> _commentsList(QJsonArray feed);

    QList<Comment*> _comments;
    QSet<int>       _ids;
    
    int             _entryId;
    bool            _loading;
    bool            _checking;
    int             _totalCount;
    
    const QString   _url;

    ApiRequest* _request;
    Entry*      _entry;
};

#endif // COMMENTSMODEL_H
