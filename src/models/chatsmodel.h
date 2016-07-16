#ifndef CHATSMODEL_H
#define CHATSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>

class Conversation;
class ApiRequest;



class ChatsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool hasMore READ hasMore NOTIFY hasMoreChanged)

public:
    static ChatsModel* instance(QObject* parent = nullptr);

    ChatsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE bool hasMore() const { return _hasMore; }

public slots:    
    void reset();

signals:
    void hasMoreChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addChats(QJsonArray data);
    void _setNotLoading(QObject* request);

private:
    QList<Conversation*> _chats;

    bool    _hasMore;
    
    QString _url;
    bool    _loading;
    int     _page;

    ApiRequest* _request;
};

#endif // CHATSMODEL_H
