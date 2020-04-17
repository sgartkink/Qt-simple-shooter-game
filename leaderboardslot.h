#ifndef LEADERBOARDSLOT_H
#define LEADERBOARDSLOT_H

#include <QObject>
#include <QGridLayout>
#include <QLabel>

class Leaderboard;
#include "herostats.h"

class LeaderboardSlot : public QObject
{
    Q_OBJECT
public:
    LeaderboardSlot(Leaderboard * lb = nullptr, HeroStats * hS = nullptr);

    void setHeroStats(HeroStats * hS);
    void updateKills();
    void updateDeaths();

private:
    QGridLayout * gridLayout = nullptr;
    HeroStats * heroStats = nullptr;

    QLabel * lName = new QLabel;
    QLabel * lKills = new QLabel;
    QLabel * lDeaths = new QLabel;

signals:
    void changed();
};

#endif // LEADERBOARDSLOT_H
