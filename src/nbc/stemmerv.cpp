// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
 * Copyright (C) 2016 Vasily Khodakov
 * Contact: <binque@ya.ru>
 *
 * This file is part of Taaasty.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stemmerv.h"

#include <QDebug>


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
    stem.remove(_reflexive);
    auto test = stem;
    test.remove(_suffix);
    if (_pvre.match(test).hasMatch())
        stem = test;
    else
        return stem;

    // Step 2
    test.remove(_plural);
    if (_pvre.match(test).hasMatch())
        stem = test;
    else
        return stem;

    // Step 3
    test.remove(_derivational);
    if (_pvre.match(test).hasMatch())
        stem = test;
    else
        return stem;

    // Step 4
    test.remove(_softSign);
    if (test == stem)
    {
        stem.remove(_perfectivePrefix);
        stem.remove(_perfectiveSuffix);
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
    , _suffix("([иы]?в(?:ши)?|[иыое][йме]|[ео]го|[ео]му|[ая]я|[еоую]?ю"
              "|[иы]х|[иы]ми|[ие]шь|[ие]т|[ие]м|[ие]те|[уюя]т|л[аои]?"
              "|[тч][ьи]|вши?|[ео]в|[ая]ми|еи|и|а|я|е|й|о|у|и?[ея]м"
              "|[ао]м|ах|и?ях|ы|ию|ь[юя]?|ия|ени|енем|от)$")
    , _plural("и$")
    , _derivational("(е?[мн]?ост|лк|(?:ль)?[нчщ]?ик|и?к|льн|ь|енн|тор"
                    "|тель|овец|ист|ец|ач|[аея]нт|[ая]не?ц|ч?[ая]н(?:ин)?"
                    "|е?н[иь]|[ая]ци|фикаци|е?ств|изм|ур|аж|ч?ик|очк|"
                    "[еи]?ц|[уыю]шк|[ео]нь?к|ищ|ующ)$")
    , _perfectivePrefix("^(наи)")
    , _perfectiveSuffix("([ае]йш)$")
    , _softSign("ь$")
    , _doubleN("нн$")
    , _separators("[^а-яА-ЯёЁa-zA-Z]+")
{
    qDebug() << "StemmerV";

    _pvre.optimize();
    _reflexive.optimize();
    _suffix.optimize();
    _plural.optimize();
    _derivational.optimize();
    _perfectivePrefix.optimize();
    _perfectiveSuffix.optimize();
    _softSign.optimize();
    _doubleN.optimize();
    _separators.optimize();
}
