#ifndef FEEDMODEL_H
#define FEEDMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>

class Entry;



class FeedModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(int  tlog READ tlog WRITE setTlog)
    Q_PROPERTY(bool hasMore MEMBER _hasMore NOTIFY hasMoreChanged)
    Q_PROPERTY(bool isPrivate READ isPrivate NOTIFY isPrivateChanged)

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

    FeedModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

    Q_INVOKABLE void setTlog(const int tlog);
    Q_INVOKABLE int tlog() const {return _tlog; }

    Q_INVOKABLE void reset(Mode mode = InvalidMode, int tlog = -1);

    Q_INVOKABLE bool isPrivate() const { return _isPrivate; }

    bool hideShort() const;

signals:
    void hasMoreChanged();
    void isPrivateChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _changeHideShort();
    void _setPrivate(int errorCode);

private:
    void _addAll(QList<Entry*>& all);
    bool _addLonger(QList<Entry*>& all);

    QList<Entry*> _entries;
    QList<Entry*> _allEntries;
    QString _url;
    int _tlog;
    Mode _mode;
    bool _hasMore;
    bool _loading;
    int _lastEntry;
    bool _isPrivate;
};

#endif // FEEDMODEL_H
