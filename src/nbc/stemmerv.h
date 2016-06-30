#ifndef STEMMERV_H
#define STEMMERV_H

#include <QRegularExpression>



// https://github.com/titarenko/ukrstemmer
// ported from https://www.drupal.org/project/ukstemmer

class StemmerV
{
public:
    static StemmerV* instance();

    QString stemWord(const QString word);
    QStringList stem(const QString text);

private:
    StemmerV();

    QRegularExpression _pvre;
    QRegularExpression _reflexive;
    QRegularExpression _suffix;
    QRegularExpression _plural;
    QRegularExpression _derivational;
    QRegularExpression _perfectivePrefix;
    QRegularExpression _perfectiveSuffix;
    QRegularExpression _softSign;
    QRegularExpression _doubleN;

    QRegularExpression _separators;
};

#endif // STEMMERV_H
