#ifndef USERSMODELTLOG_H
#define USERSMODELTLOG_H

#include "usersmodel.h"



class UsersModelTlog : public UsersModel
{
    Q_OBJECT

    friend class UsersModelBayes;

    Q_PROPERTY(int tlog READ tlog WRITE setTlog)

public:
    UsersModelTlog(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE virtual void setMode(const Mode mode) override;

    Q_INVOKABLE void setTlog(const int tlog);
    Q_INVOKABLE int tlog() const {return _tlog; }

    void downloadAll();

signals:
    void hasMoreChanged();
    void downloadCompleted();

private slots:
    void _addItems(QJsonObject data);

private:
    bool _loadAll;

    QList<User*> _users;
    QString _url;
    QString _field;
    int  _tlog;
    Mode _mode;
    int  _total;
//    bool _loading;
    int  _lastPosition;
};

#endif // USERSMODELTLOG_H
