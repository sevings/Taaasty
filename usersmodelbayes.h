#ifndef USERSMODELBAYES_H
#define USERSMODELBAYES_H

#include "usersmodeltlog.h"

class Trainer;



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
    void _loadBayesTlogs();

    Trainer*        _trainer;
    UsersModelTlog* _tlogModel;
};

#endif // USERSMODELBAYES_H
