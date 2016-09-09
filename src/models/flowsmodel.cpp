#include "flowsmodel.h"

#include <QDebug>
#include <QJsonArray>

#include "../defines.h"

#include "../tasty.h"
#include "../apirequest.h"
#include "../data/Flow.h"



FlowsModel::FlowsModel(QObject* parent)
    : QAbstractListModel(parent)
    , _page(1)
    , _hasMore(true)
    , _loading(false)
    , _request(nullptr)
{
    qDebug() << "FlowsModel";

    setMode(Tasty::instance()->isAuthorized() ? MyMode : PopularMode);
}



int FlowsModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return _flows.size();
}



QVariant FlowsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _flows.size())
        return QVariant();

    if (role == Qt::UserRole)
        return QVariant::fromValue<Flow*>(_flows.at(index.row()));

    qDebug() << "role" << role;

    return QVariant();
}



bool FlowsModel::canFetchMore(const QModelIndex& parent) const
{
    if (parent.isValid())
        return false;

    return _hasMore;
}



void FlowsModel::fetchMore(const QModelIndex& parent)
{
    if (!_hasMore || _loading || parent.isValid())
        return;

    qDebug() << "FlowsModel::fetchMore";

    _loading = true;
    emit loadingChanged();

    QString url = _url.arg(_page++);
    _request = new ApiRequest(url);

    Q_TEST(connect(_request, SIGNAL(success(QJsonObject)), this, SLOT(_addItems(QJsonObject))));
    Q_TEST(connect(_request, SIGNAL(destroyed(QObject*)),  this, SLOT(_setNotLoading(QObject*))));
}



void FlowsModel::setMode(const FlowsModel::Mode mode)
{
    beginResetModel();

    _mode = mode;

    switch(_mode)
    {
    case PopularMode:
        _url = "v1/flows.json?sort=popular&page=%1&limit=20";
        disconnect(Tasty::instance(), SIGNAL(authorized()), this, SLOT(reset()));
        break;
    case NewestMode:
        _url = "v1/flows.json?sort=newest&page=%1&limit=20";
        disconnect(Tasty::instance(), SIGNAL(authorized()), this, SLOT(reset()));
        break;
    case MyMode:
        _url = "v1/flows.json?sort=my&page=%1&limit=20";
        Q_TEST(connect(Tasty::instance(), SIGNAL(authorized()), this, SLOT(reset())));
        break;
    default:
        qDebug() << "Flows mode:" << _mode;
    }

    qDeleteAll(_flows);
    _flows.clear();

    _page = 1;
    _hasMore = true;
    emit hasMoreChanged();

    _loading = false;
    emit loadingChanged();

    delete _request;
    _request = nullptr;

    endResetModel();
}



void FlowsModel::reset()
{
    beginResetModel();

    qDeleteAll(_flows);
    _flows.clear();

    _page = 1;
    _hasMore = true;
    emit hasMoreChanged();

    _loading = false;
    emit loadingChanged();

    delete _request;
    _request = nullptr;

    endResetModel();
}



QHash<int, QByteArray> FlowsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole] = "flow";
    return roles;
}



void FlowsModel::_addItems(QJsonObject data)
{
    qDebug() << "FlowsModel::_addItems";

    _request = nullptr;

    auto hasMore = data.value("has_more").toBool();
    if (hasMore != _hasMore)
    {
        _hasMore = hasMore;
        emit hasMoreChanged();
    }

    auto flows = data.value("items").toArray();
    if (flows.isEmpty())
    {
        _loading = false;
        emit loadingChanged();
        return;
    }

    beginInsertRows(QModelIndex(), _flows.size(), _flows.size() + flows.size() - 1);

    foreach (auto flowData, flows)
        _flows << new Flow(flowData.toObject().value("flow").toObject(), this);

    endInsertRows();

    _loading = false;
    emit loadingChanged();
}



void FlowsModel::_setNotLoading(QObject* request)
{
    if (request != _request)
        return;

    if (_loading)
    {
        _loading = false;
        emit loadingChanged();
    }

    _request = nullptr;
}



bool FlowsModel::hasMore() const
{
    return _hasMore;
}



bool FlowsModel::loading() const
{
    return _loading;
}
