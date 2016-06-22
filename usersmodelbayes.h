#ifndef USERSMODELBAYES_H
#define USERSMODELBAYES_H

#include "usersmodeltlog.h"



class UsersModelBayes : public UsersModel
{
    Q_OBJECT

    friend class Trainer;

public:
    UsersModelBayes(QObject* parent = nullptr);
    ~UsersModelBayes();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE virtual bool hasMore() const override { return _loading; }

    Q_INVOKABLE virtual void setMode(const Mode mode) override;

    Q_INVOKABLE void removeUser(int id);

private slots:
    void _setBayesItems();

private:
    struct BayesTlog {
        BayesTlog(int userId = 0, int last = 0);
        BayesTlog(User* user);
        ~BayesTlog();
        inline bool operator==(const BayesTlog& other);
        inline bool operator==(const int& userId);
        void loadInfo();
        User* user;
        int id;
        int latest;
    };

    int _findTlog(int& type, int id);

    void _initDb();
    void _loadDb();
    void _saveDb();

    void _loadBayesTlogs();

    QList<BayesTlog> _tlogs[2];
    UsersModelTlog* _tlogModel;
};

#endif // USERSMODELBAYES_H
