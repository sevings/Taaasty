#ifndef COMMENTSMODEL_H
#define COMMENTSMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>

#include "datastructures.h"



class CommentsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int entryId READ entryId WRITE setEntryId)
    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)

public:
    enum CommentRoles {
        CommentIdRole = Qt::UserRole + 1,
        UserIdRole,
        UserNameRole,
        UserUrlRole,
        Thumb64Role,
        Thumb128Role,
        SymbolRole,
        CommentHtmlRole,
        CreatedAtRole,
        EditableRole,
        DeletableRole,
        CommentObjectRole
    };

    explicit CommentsModel(QObject *parent = 0);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int entryId() const { return _entryId; }
    Q_INVOKABLE void setEntryId(const int id);

    Q_INVOKABLE bool hasMore() const { return _comments.size() < _totalCount; }

signals:
    void hasMoreChanged();

public slots:
    void loadMore();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addComments(QJsonObject data);

private:
    QList<Comment*> _comments;
    int _entryId;
    bool _loading;
    int _toComment;
    int _totalCount;
    const QString _url;
};

#endif // COMMENTSMODEL_H
