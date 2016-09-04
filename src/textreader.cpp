#include "textreader.h"

#include <QFile>
#include <QDebug>


TextReader::TextReader(QObject *parent)
    : QObject(parent)
{

}



QString TextReader::read()
{
    if (_source.isEmpty()){
        qDebug() << "text file source is empty";
        return QString();
    }

    QFile file(_source);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Unable to open the file" << _source << file.errorString();
        return QString();
    }

    QString line;
    QString fileContent;
    QTextStream t( &file );
    do {
        line = t.readLine();
        fileContent += line;
     } while (!line.isNull());

    file.close();

    return fileContent;
}
