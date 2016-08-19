#ifndef CHATSMODEL_H
#define CHATSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QHash>

class Conversation;
class ApiRequest;
class Entry;



class ChatsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode    READ mode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool hasMore READ hasMore            NOTIFY hasMoreChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    enum Mode {
        AllChatsMode     = 0,
        PrivateChatsMode = 1,
        EntryChatsMode   = 2
    };

    Q_ENUMS(Mode)
    
    static ChatsModel* instance(QObject* parent = nullptr);

    ChatsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE bool hasMore() const { return _hasMore; }

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

    void addChat(Entry* entry);

    bool loading() const;

public slots:
    void loadUnread();
    void reset();

signals:
    void hasMoreChanged();
    void loadingChanged();
    void modeChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addUnread(QJsonArray data);
    void _addChats(QJsonArray data);
    void _setNotLoading(QObject* request);
    void _removeChat(int id);
    void _checkUnread(int actual);

private:
    void _bubbleChat(int id);

    Conversation* _chat(int id) const;

    QList<int>           _allChats;
    QList<int>           _chats;
    QSet<int>            _ids;
    Mode                 _mode;

    bool    _hasMore;
    
    QString _url;
    bool    _loading;
    int     _page;

    ApiRequest* _request;
};

#endif // CHATSMODEL_H
