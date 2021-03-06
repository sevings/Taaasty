#ifndef TASTYLISTMODEL_H
#define TASTYLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QPointer>
#include <QSortFilterProxyModel>

#include "../defines.h"
#include "../apirequest.h"



class TastyListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading         READ isLoading      NOTIFY loadingChanged)
    Q_PROPERTY(bool checking        READ isChecking     NOTIFY checkingChanged)
    Q_PROPERTY(bool hasMore         READ hasMore        NOTIFY hasMoreChanged)
    Q_PROPERTY(int  size            READ rowCount       NOTIFY rowCountChanged)
    Q_PROPERTY(bool networkError    READ networkError   NOTIFY networkErrorChanged)
    Q_PROPERTY(QString errorString  READ errorString    NOTIFY errorStringChanged)

public:
    TastyListModel(QObject* parent = nullptr);

    void setFilter(QSortFilterProxyModel* filter);

    virtual bool isLoading() const;
    bool isChecking() const;

    virtual bool hasMore() const;

    bool networkError() const;
    QString errorString() const;

signals:
    void loadingChanged();
    void checkingChanged();
    void hasMoreChanged();
    void rowCountChanged();
    void errorStringChanged();
    void networkErrorChanged();

public slots:
    virtual void loadMore();
    void pendingSort();
    
protected slots:
    void _setErrorString(int errorCode, QString str);

private slots:
    void _sort();

protected:
    void                 _initLoad();
    void                 _initCheck();

    ApiRequest::Options  _optionsForFetchMore(bool accessTokenRequired = true) const;

    QPointer<ApiRequest> _loadRequest;
    QPointer<ApiRequest> _checkRequest;
    bool                 _hasMore;
    bool                 _networkError;
    QString              _errorString;

    QSortFilterProxyModel* _filter; //-V122
    bool _sorted;
};

#endif // TASTYLISTMODEL_H
