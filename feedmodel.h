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

    bool hideShort() const;

signals:
    void hasMoreChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _changeHideShort();

private:
    QList<Entry*> _entries;
    QList<Entry*> _allEntries;
    QString _url;
    int _tlog;
    Mode _mode;
    bool _hasMore;
    bool _loading;
    int _lastEntry;
};

#endif // FEEDMODEL_H
