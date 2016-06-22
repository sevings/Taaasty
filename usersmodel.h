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

    Q_PROPERTY(Mode mode READ mode WRITE setMode)
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

    Q_INVOKABLE virtual bool hasMore() const { return canFetchMore(QModelIndex()); }

    Q_INVOKABLE virtual void setMode(const Mode mode) { _mode = mode; }
    Q_INVOKABLE Mode mode() const {return _mode; }

signals:
    void hasMoreChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

    Mode _mode;
    bool _loading;
};

#endif // USERSMODEL_H
