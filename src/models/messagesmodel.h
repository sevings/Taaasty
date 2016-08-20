#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

class ApiRequest;
class Conversation;
class Message;



class MessagesModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int chatId           READ chatId)
    Q_PROPERTY(bool hasMore         READ hasMore    NOTIFY hasMoreChanged)
    Q_PROPERTY(bool loading         READ loading    NOTIFY loadingChanged)

public:
    explicit MessagesModel(Conversation* chat = nullptr);

    void init(Conversation* chat);

    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE int chatId() const { return _chatId; }
    Q_INVOKABLE void reset();

    Q_INVOKABLE bool hasMore() const { return _messages.size() < _totalCount; }
    Q_INVOKABLE bool loading() const { return _loading; }

    Q_INVOKABLE void check();

    Message* lastMessage() const;

signals:
    void hasMoreChanged();
    void loadingChanged();
    void totalCountChanged(int tc);
    void lastMessageChanged();

public slots:
    void loadMore();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addMessages(const QJsonObject data);
    void _addLastMessages(const QJsonObject data);
    void _addMessage(const QJsonObject data);
    void _addMessage(const int chatId, const QJsonObject data);
    void _removeMessage(QObject* msg);
    void _setNotLoading(QObject* request);

private:
    void            _setTotalCount(int tc);
    QList<Message*> _messagesList(QJsonArray feed);

    QList<Message*> _messages;
    QSet<int>       _ids;
    Conversation*   _chat;
    
    int             _chatId;
    bool            _loading;
    int             _totalCount;
    
    const QString   _url;

    ApiRequest* _request;

};

#endif // MESSAGESMODEL_H
