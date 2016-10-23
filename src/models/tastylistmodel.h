#ifndef TASTYLISTMODEL_H
#define TASTYLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QPointer>

#include "../apirequest.h"



class TastyListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(bool loading         READ isLoading      NOTIFY loadingChanged)
    Q_PROPERTY(bool checking        READ isChecking     NOTIFY checkingChanged)
    Q_PROPERTY(bool hasMore         READ hasMore        NOTIFY hasMoreChanged)
    Q_PROPERTY(QString errorString  READ errorString    NOTIFY errorStringChanged)

public:
    TastyListModel(QObject* parent = nullptr);

    bool isLoading() const;
    bool isChecking() const;

    virtual bool hasMore() const;

    QString errorString() const;

signals:
    void loadingChanged();
    void checkingChanged();
    void hasMoreChanged();
    void errorStringChanged();

protected slots:
    void _setErrorString(int errorCode, QString str);

protected:
    void                 _initLoad(bool emitting = true);
    void                 _initCheck(bool emitting = true);

    ApiRequest::Options  _optionsForFetchMore(bool accessTokenRequired = true) const;

    QPointer<ApiRequest> _loadRequest;
    QPointer<ApiRequest> _checkRequest;
    bool                 _hasMore;
    QString              _errorString;

private:
};

#endif // TASTYLISTMODEL_H
