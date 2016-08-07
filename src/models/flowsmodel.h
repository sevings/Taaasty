#ifndef FLOWSMODEL_H
#define FLOWSMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QJsonObject>

class Flow;
class ApiRequest;



class FlowsModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(Mode mode READ mode WRITE setMode)
    Q_PROPERTY(bool hasMore MEMBER _hasMore NOTIFY hasMoreChanged)

public:
    enum Mode
    {
        AllFlowsMode,
        MyFlowsMode
    };

    Q_ENUMS(Mode)

    FlowsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool canFetchMore(const QModelIndex& parent) const override;
    void fetchMore(const QModelIndex& parent) override;

    Q_INVOKABLE void setMode(const Mode mode);
    Q_INVOKABLE Mode mode() const {return _mode; }

signals:
    void hasMoreChanged();

public slots:
    void reset();

protected:
    QHash<int, QByteArray> roleNames() const override;

private slots:
    void _addItems(QJsonObject data);
    void _setNotLoading(QObject* request);

private:
    QList<Flow*> _flows;
    QString      _url;
    Mode         _mode;
    int          _page;
    bool         _hasMore;

    bool         _loading;
    ApiRequest*  _request;
};

#endif // FLOWSMODEL_H
