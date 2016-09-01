#include "Rating.h"

#include <QtConcurrent>

#include "../defines.h"

#include "../apirequest.h"
#include "../nbc/bayes.h"
#include "Entry.h"



Rating::Rating(QObject* parent)
    : QObject(parent)
    , _entryId(0)
    , _votes(0)
    , _rating(0)
    , _isVoted(false)
    , _isVotable(false)
    , _bayesRating(0)
    , _isBayesVoted(false)
    , _isVotedAgainst(false)
    , _parent(qobject_cast<Entry*>(parent))
{

}



Rating::Rating(const QJsonObject data, Entry* parent)
    : QObject(parent)
    , _entryId(0)
    , _bayesRating(0)
    , _isBayesVoted(false)
    , _isVotedAgainst(false)
    , _parent(parent)
{
    init(data);

    Q_TEST(connect(&_watcher, SIGNAL(finished()), this, SLOT(_changeBayesRating())));
}



Rating::~Rating()
{
    if (!_watcher.isFinished())
        _watcher.waitForFinished();
}



void Rating::reCalcBayes()
{
    auto type = Bayes::instance()->entryVoteType(_entryId);
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
//    auto future = QtConcurrent::run(Bayes::instance(), &Bayes::classify, _parent, 0);
//    _watcher.setFuture(future);
}



void Rating::vote()
{
    voteBayes();

    if (!_isVotable || !Tasty::instance()->isAuthorized())
        return;

    auto url = QString("v1/entries/%1/votes.json").arg(_entryId);
    auto operation = (_isVoted ? QNetworkAccessManager::DeleteOperation
                               : QNetworkAccessManager::PostOperation);
    auto request = new ApiRequest(url, true, operation);

    Q_TEST(connect(request, SIGNAL(success(const QJsonObject)),
                   this, SLOT(init(const QJsonObject))));
}



void Rating::voteBayes()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    auto future = QtConcurrent::run(Bayes::instance(), &Bayes::voteForEntry, _parent, Bayes::Fire);
    _watcher.setFuture(future);

    _isBayesVoted = true;
    emit bayesChanged();
}



void Rating::voteAgainst()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    auto future = QtConcurrent::run(Bayes::instance(), &Bayes::voteForEntry, _parent, Bayes::Water);
    _watcher.setFuture(future);

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



void Rating::_changeBayesRating()
{
    _bayesRating = _watcher.result();

    emit bayesChanged();
}



int Rating::bayesRating() const
{
    return _bayesRating;
}
