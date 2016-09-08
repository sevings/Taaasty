#pragma once

#include <QObject>
#include <QPointer>

class ApiRequest;



class TastyData: public QObject
{
    Q_OBJECT

    Q_PROPERTY(int  id      READ id        NOTIFY idChanged)
    Q_PROPERTY(bool loading READ isLoading NOTIFY loadingChanged)

public:
    TastyData(QObject* parent = nullptr);

    int id() const;
    bool isLoading() const;

signals:
    void idChanged();
    void loadingChanged();

protected:
    int _id;
    QPointer<ApiRequest> _request;
    
    void _initRequest(bool emitting = true);
};
