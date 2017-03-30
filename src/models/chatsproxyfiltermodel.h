#ifndef CHATSPROXYFILTERMODEL_H
#define CHATSPROXYFILTERMODEL_H

#include <QSortFilterProxyModel>

#include "chatsmodel.h"



class ChatsProxyFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode NOTIFY modeChanged)

    Q_PROPERTY(bool loading         READ isLoading      NOTIFY loadingChanged)
    Q_PROPERTY(bool checking        READ isChecking     NOTIFY checkingChanged)
    Q_PROPERTY(bool hasMore         READ hasMore        NOTIFY hasMoreChanged)
    Q_PROPERTY(int  size            READ rowCount       NOTIFY rowCountChanged)
    Q_PROPERTY(bool networkError    READ networkError   NOTIFY networkErrorChanged)
    Q_PROPERTY(QString errorString  READ errorString    NOTIFY errorStringChanged)

public:
    enum Mode {
        AllChatsMode     = 0,
        PrivateChatsMode = 1,
        EntryChatsMode   = 2
    };

    Q_ENUMS(Mode)

    ChatsProxyFilterModel(QObject* parent = nullptr);

    void setMode(const Mode mode);
    Mode mode() const {return _mode; }

    bool isLoading() const;
    bool isChecking() const;
    bool hasMore() const;
    bool networkError() const;
    QString errorString() const;

signals:
    void modeChanged();

    void loadingChanged();
    void checkingChanged();
    void hasMoreChanged();
    void rowCountChanged();
    void errorStringChanged();
    void networkErrorChanged();

public slots:
    void loadLast();
    void loadUnread();
    void reset();

    void loadMore();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

private:
    Mode _mode;
};

#endif // CHATSPROXYFILTERMODEL_H
