#include "characters/herostats.h"
#include "ui/leaderboardslot.h"
#include <QtDebug>
HeroStats::HeroStats(Hero * owner)
    : owner(owner)
{

}

void HeroStats::increaseKills()
{
    kills++;
    if (slot)       // a hero that isn't on a leaderboard still counts
        slot->updateKills();
}

void HeroStats::increaseDeath()
{
    deaths++;
    if (slot)
        slot->updateDeaths();
}
