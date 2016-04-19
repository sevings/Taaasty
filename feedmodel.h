#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>



class FeedModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(int tlog READ tlog WRITE setTlog)

public:
    enum Mode {
        MyTlogMode,
        FriendsMode,
        LiveMode,
        AnonymousMode,
        BestMode,
        ExcellentMode,
        WellMode,
        GoodMode,
        TlogMode,
        FavoritesMode
    };

    Q_ENUMS(Mode)

    enum FeedRoles {
        IdRole = Qt::UserRole + 1,
        CreatedAtRole,
        UrlRole,
        TypeRole,
        VotableRole,
        PrivateRole,
        TlogRole,
        AuthorRole,
        RatingRole,
        CommentsCountRole,
        TitleRole,
        TruncatedTitleRole,
        TextRole,
        TruncatedTextRole,
        ImageAttachRole,
        ImagePreviewRole
    };

    FeedModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

    Q_INVOKABLE void setTlog(const int tlog);
    Q_INVOKABLE int tlog() const {return _tlog; }

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);

private:
    QList<QJsonObject> _items;
    QString _url;
    int _tlog;
    Mode _mode;
    bool _hasMore;
    bool _loading;
    int _lastEntry;
};

#endif // FEEDMODEL_H
