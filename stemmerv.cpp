#include "stemmerv.h"



StemmerV* StemmerV::instance()
{
    static auto stemmer = new StemmerV;
    return stemmer;
}



QString StemmerV::stemWord(const QString word)
{
    if (word.isEmpty())
        return word;

    auto stem = word.trimmed().toLower().replace("ё", "е");

    if (!_pvre.match(stem).hasMatch())
        return stem;

    // Step 1
    stem.replace(_reflexive, QString());
    auto test = stem;
    test.replace(_suffix, QString());
    if (_pvre.match(test).hasMatch())
        stem = test;
    else
        return stem;

    // Step 2
    test.replace(_plural, QString());
    if (_pvre.match(test).hasMatch())
        stem = test;
    else
        return stem;

    // Step 3
    test.replace(_derivational, QString());
    if (_pvre.match(test).hasMatch())
        stem = test;
    else
        return stem;

    // Step 4
    test.replace(_softSign, QString());
    if (test == stem)
    {
        stem.replace(_perfectivePrefix, QString());
        stem.replace(_perfectiveSuffix, QString());
        stem.replace(_doubleN, "н");
    }
    else
        stem = test;

    return stem;
}



QStringList StemmerV::stem(const QString text)
{
    auto words = text.split(_separators, QString::SkipEmptyParts);
    QStringList stemmed;
    stemmed.reserve(words.size());

    foreach (auto word, words)
        stemmed << stemWord(word);

    return stemmed;
}



StemmerV::StemmerV()
    : _pvre("^(.*?[уеыаоэяию])(.*)$")
    , _reflexive("(с[яьи])$")
    , _suffix("([иы]?в(?:ши)?|[иыое][йме]|[ео]го|[ео]му|[ая]я|[еоую]?ю|[иы]х|[иы]ми|[ие]шь|[ие]т|[ие]м|[ие]те|[уюя]т|л[аои]?|[тч][ьи]|вши?|[ео]в|[ая]ми|еи|и|а|я|е|й|о|у|и?[ея]м|[ао]м|ах|и?ях|ы|ию|ь[юя]?|ия|ени|енем|от)$")
    , _plural("и$")
    , _derivational("(е?[мн]?ост|лк|(?:ль)?[нчщ]?ик|и?к|льн|ь|енн|тор|тель|овец|ист|ец|ач|[аея]нт|[ая]не?ц|ч?[ая]н(?:ин)?|е?н[иь]|[ая]ци|фикаци|е?ств|изм|ур|аж|ч?ик|очк|[еи]?ц|[уыю]шк|[ео]нь?к|ищ|ующ)$")
    , _perfectivePrefix("^(наи)")
    , _perfectiveSuffix("([ае]йш)$")
    , _softSign("ь$")
    , _doubleN("нн$")
    , _separators("[^а-яА-ЯёЁa-zA-Z]+")
{
//    _pvre.optimize();
//    _reflexive.optimize();
//    _suffix.optimize();
//    _plural.optimize();
//    _derivational.optimize();
//    _perfectivePrefix.optimize();
//    _perfectiveSuffix.optimize();
//    _softSign.optimize();
//    _doubleN.optimize();
//    _separators.optimize();
}
