#include "Rating.h"

#include <QtConcurrent>

#include "../defines.h"

#include "../apirequest.h"
#include "../tasty.h"
#include "../settings.h"

#include "Entry.h"
#include "Author.h"



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

    auto future = QtConcurrent::run(this, &Rating::_changeBayesRating, Bayes::Fire);
    _watcher.setFuture(future);

    _isBayesVoted = true;
}



void Rating::voteAgainst()
{
    if (_isBayesVoted || _isVotedAgainst)
        return;

    auto future = QtConcurrent::run(this, &Rating::_changeBayesRating, Bayes::Water);
    _watcher.setFuture(future);

    _isVotedAgainst = true;
}



void Rating::init(const QJsonObject data)
{
    int id = data.value("entry_id").toInt();
    if (id)
        _entryId = id;

    _votes      = data.value("votes").toInt();
    _rating     = data.value("rating").toInt();
    _isVoted    = data.value("is_voted").toBool();
    _isVotable  = data.value("is_voteable").toBool() && (!_parent || (_parent->type() != "anonymous"
            && _parent->author()->id() != Tasty::instance()->settings()->userId()));

    emit dataChanged();
}



void Rating::_changeBayesRating(Bayes::Type type)
{
    _bayesRating = Bayes::instance()->voteForEntry(_parent, type);

    emit bayesChanged();
}



int Rating::bayesRating() const
{
    return _bayesRating;
}
