#pragma once

#include <QObject>
#include <QJsonObject>
#include <QFutureWatcher>

class Entry;



class Rating: public QObject
{
    Q_OBJECT

    Q_PROPERTY (int  entryId    MEMBER _entryId     NOTIFY dataChanged)
    Q_PROPERTY (int  votes      MEMBER _votes       NOTIFY dataChanged)
    Q_PROPERTY (int  rating     MEMBER _rating      NOTIFY dataChanged)
    Q_PROPERTY (bool isVoted    MEMBER _isVoted     NOTIFY dataChanged)
    Q_PROPERTY (bool isVotable  MEMBER _isVotable   NOTIFY dataChanged)

    Q_PROPERTY (int  bayesRating    MEMBER _bayesRating     NOTIFY bayesChanged)
    Q_PROPERTY (bool isBayesVoted   MEMBER _isBayesVoted    NOTIFY bayesChanged)
    Q_PROPERTY (bool isVotedAgainst MEMBER _isVotedAgainst  NOTIFY bayesChanged)

public:
    Rating(const QJsonObject data = QJsonObject(), Entry* parent = nullptr);
    ~Rating();

    void reCalcBayes();

    int bayesRating() const;

public slots:
    void vote();
    void voteBayes();
    void voteAgainst();

    void init(const QJsonObject data);

signals:
    void dataChanged();
    void bayesChanged();

private slots:
    void _changeBayesRating();

private:
    int  _entryId;
    int  _votes;
    int  _rating;
    bool _isVoted;
    bool _isVotable;

    int  _bayesRating;
    bool _isBayesVoted;
    bool _isVotedAgainst;

    Entry* _parent;

    QFutureWatcher<int> _watcher;
};
