#ifndef TEXTREADER_H
#define TEXTREADER_H

#include <QObject>

class TextReader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString source MEMBER _source NOTIFY sourceChanged)

public:
    explicit TextReader(QObject *parent = 0);

    Q_INVOKABLE QString read();

signals:
    void sourceChanged(const QString& source);

private:
    QString _source;
};

#endif // TEXTREADER_H
