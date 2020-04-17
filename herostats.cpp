#include "herostats.h"
#include "leaderboardslot.h"
#include <QtDebug>
HeroStats::HeroStats(Hero * owner)
    : owner(owner)
{

}

void HeroStats::increaseKills()
{
    kills++;
    slot->updateKills();
}

void HeroStats::increaseDeath()
{
    deaths++;
    slot->updateDeaths();
}
