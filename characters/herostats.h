#ifndef HEROSTATS_H
#define HEROSTATS_H

class Hero;
class LeaderboardSlot;
#include <QString>

class HeroStats
{
public:
    HeroStats(Hero * owner);

    void setSlot(LeaderboardSlot * lS) { slot = lS; }
    void setName(QString n) { name = n; }
    void increaseKills();
    void increaseDeath();

    QString getName() { return name; }
    unsigned int getKills() const { return kills; }
    unsigned int getDeaths() const { return deaths; }

private:
    const Hero * owner;
    QString name;
    unsigned int kills = 0;
    unsigned int deaths = 0;

    LeaderboardSlot * slot = nullptr;
};

#endif // HEROSTATS_H
