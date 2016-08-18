#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>

class Entry;
class ApiRequest;



class FeedModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode     mode        READ mode       WRITE setMode)
    Q_PROPERTY(int      tlog        READ tlog       WRITE setTlog)
    Q_PROPERTY(QString slug         READ slug       WRITE setSlug)
    Q_PROPERTY(bool     hasMore     READ hasMore                    NOTIFY hasMoreChanged)
    Q_PROPERTY(bool     loading     READ loading                    NOTIFY loadingChanged)
    Q_PROPERTY(bool     isPrivate   READ isPrivate                  NOTIFY isPrivateChanged)
    Q_PROPERTY(int      minRating   READ minRating  WRITE setMinRating)
    Q_PROPERTY(QString  query       READ query      WRITE setQuery  NOTIFY queryChanged)

public:
    enum Mode {
        InvalidMode,
        MyTlogMode,
        FriendsMode,
        LiveMode,
        AnonymousMode,
        BestMode,
        ExcellentMode,
        WellMode,
        GoodMode,
        BetterThanMode,
        TlogMode,
        FavoritesMode
    };

    Q_ENUMS(Mode)

    enum Privacy
    {
        Private,
        Public,
        Voting
    };

    Q_ENUMS(Privacy)

    FeedModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

    Q_INVOKABLE void setTlog(const int tlog);
    Q_INVOKABLE int tlog() const {return _tlog; }

    Q_INVOKABLE void setSlug(const QString slug);
    Q_INVOKABLE QString slug() const { return _slug; }

    Q_INVOKABLE void setMinRating(const int rating);
    Q_INVOKABLE int minRating() const { return _minRating; }

    Q_INVOKABLE void setQuery(const QString query);
    Q_INVOKABLE QString query() const { return _query; }

    Q_INVOKABLE void reset(Mode mode = InvalidMode, int tlog = -1,
                           QString slug = QString(), QString query = QString());

    Q_INVOKABLE bool isPrivate() const { return _isPrivate; }

    bool hideMode() const;
    bool hideShort() const;
    bool hideNegative() const;
    bool loading() const;
    bool hasMore() const;

    void postText(const QString title, const QString content, Privacy privacy = Public);
    void postAnonymous(const QString title, const QString content);

    Q_INVOKABLE void setSinceEntryId(int id);

signals:
    void hasMoreChanged();
    void loadingChanged();
    void isPrivateChanged();
    void queryChanged();
    void entryCreated(Entry* entry);

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _addNewPost(QJsonObject data);
    void _changeHideSome();
    void _setPrivate(int errorCode);
    void _setNotLoading(QObject* request);
    void _reloadRatings();
    void _setRatings(const QJsonArray data);

private:
    void _addAll(QList<Entry*>& all);
    bool _addSome(QList<Entry*>& all);

    void _setUrl(Mode mode);

    QList<Entry*> _entries;
    QList<Entry*> _allEntries;
    QString _url;
    int _tlog;
    QString _slug;
    Mode _mode;
    bool _hasMore;
    bool _loading;
    int _lastEntry;
    bool _isPrivate;
    int _minRating;
    QString _query;
    int     _page;

    ApiRequest* _request;
};

#endif // FEEDMODEL_H
