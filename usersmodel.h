#ifndef USERSMODEL_H
#define USERSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>

class User;
class Bayes;



class UsersModel : public QAbstractListModel
{
    Q_OBJECT

    friend class Trainer;

    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(int  tlog READ tlog WRITE setTlog)
    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)

public:
    enum Mode {
        WaterMode,
        FireMode,
        FollowingsMode,
        FollowersMode,
        MyFollowingsMode,
        MyFollowersMode,
        MyIgnoredMode
    };

    Q_ENUMS(Mode)

    UsersModel(QObject* parent = nullptr);
    ~UsersModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE bool hasMore() const { return canFetchMore(QModelIndex()); }

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

    Q_INVOKABLE void setTlog(const int tlog);
    Q_INVOKABLE int tlog() const {return _tlog; }

    void downloadAll();

signals:
    void hasMoreChanged();
    void downloadCompleted();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _setBayesItems();

private:
    struct BayesTlog {
        BayesTlog(int userId = 0, int last = 0);
        BayesTlog(User* user);
        ~BayesTlog();
        void loadInfo();
        User* user;
        int id;
        int latest;
        bool include;
        bool removed;
    };
    BayesTlog _findTlog(int id, bool included = false);

    void _initDb();
    void _loadDb();
    void _saveDb();

    void _loadBayesTlogs();

    QList<BayesTlog> _tlogs[2];
    bool _loadAll;
    UsersModel* _bayesModel;

    QList<User*> _users;
    QString _url;
    QString _field;
    int  _tlog;
    Mode _mode;
    int  _total;
    bool _loading;
    int  _lastPosition;
};

#endif // USERSMODEL_H
