#include "Rating.h"

#include "./defines.h"

#include "../apirequest.h"
#include "../bayes.h"
#include "data/Entry.h"



Rating::Rating(const QJsonObject data, Entry* parent)
    : QObject(parent)
    , _entryId(0)
    , _bayesRating(0)
    , _isBayesVoted(false)
    , _isVotedAgainst(false)
    , _parent(parent)
{
    init(data);

    Q_TEST(connect(Tasty::instance(), SIGNAL(ratingChanged(QJsonObject)), this, SLOT(init(QJsonObject))));
}



void Rating::reCalcBayes()
{
    auto type = Bayes::instance()->entryVoteType(_parent->entryId());
    switch (type)
    {
    case Bayes::Water:
        _isBayesVoted = false;
        _isVotedAgainst = true;
        break;
    case Bayes::Fire:
        _isBayesVoted = true;
        _isVotedAgainst = false;
        break;
    default:
        _isBayesVoted = false;
        _isVotedAgainst = false;

        if (_isVoted)
        {
            voteBayes();
            return;
        }
    }

    _bayesRating = Bayes::instance()->classify(_parent);
    emit bayesChanged();
}



void Rating::vote()
{
    voteBayes();

    if (!_isVotable || !Tasty::instance()->isAuthorized())
        return;

    auto url = QString("entries/%1/votes.json").arg(_entryId);
    auto operation = (_isVoted ? QNetworkAccessManager::DeleteOperation
                               : QNetworkAccessManager::PostOperation);
    auto request = new ApiRequest(url, true, operation);

    connect(request, SIGNAL(success(const QJsonObject)),
            this, SLOT(init(const QJsonObject)));
}



void Rating::voteBayes()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    _bayesRating = Bayes::instance()->voteForEntry(_parent, Bayes::Fire);
    _isBayesVoted = true;

    emit bayesChanged();
}



void Rating::voteAgainst()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    _bayesRating = Bayes::instance()->voteForEntry(_parent, Bayes::Water);
    _isVotedAgainst = true;

    emit bayesChanged();
}



void Rating::init(const QJsonObject data)
{
    int id = data.value("entry_id").toInt();
    if (id)
        _entryId = id;

    _votes      = data.value("votes").toInt();
    _rating     = data.value("rating").toInt();
    _isVoted    = data.value("is_voted").toBool();
    _isVotable  = data.value("is_voteable").toBool();

    emit dataChanged();
}



int Rating::bayesRating() const
{
    return _bayesRating;
}
