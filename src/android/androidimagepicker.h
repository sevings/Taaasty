#ifndef ANDROIDIMAGEPICKER_H
#define ANDROIDIMAGEPICKER_H

#include <QObject>
#include <QAndroidJniEnvironment>
#include <QAndroidActivityResultReceiver>

#include "../defines.h"



class AndroidImagePicker: public QAndroidActivityResultReceiver
{
public:
    AndroidImagePicker(QObject* parent);

    void select();

    virtual void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject& data) override;

private:
    bool _catchException(QAndroidJniEnvironment& env) const;
    
    QObject* _parent;
};

#endif // ANDROIDIMAGEPICKER_H
