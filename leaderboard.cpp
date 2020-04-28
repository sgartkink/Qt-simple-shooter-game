#include "leaderboard.h"
#include <QtDebug>
Leaderboard::Leaderboard(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setVisible(0);
    setAttribute(Qt::WA_TransparentForMouseEvents);
//    setGeometry(0, 0, 300, 100);
    setGraphicsEffect(effect);
    effect->setOpacity(0.7);

    setLayout(gridLayout);
    gridLayout->setAlignment(Qt::AlignCenter);

    gridLayout->addWidget(&lNr, 0, 0);
    gridLayout->addWidget(&lPlayer, 0, 1);
    gridLayout->addWidget(&lKills, 0, 2);
    gridLayout->addWidget(&lDeaths, 0, 3);
}

void Leaderboard::addNewHero(HeroStats *hS)
{
    heroes.push_back(hS);

    LeaderboardSlot * leaderboardSlot = new LeaderboardSlot(this, hS);
    leaderboardSlots.push_back(leaderboardSlot);

    sortHeroes();
    update();
}

void Leaderboard::update()
{
    sortHeroes();
    updateSlots();
}

void Leaderboard::sortHeroes()
{
    std::sort(heroes.begin(), heroes.end(), [](HeroStats * lhs, HeroStats * rhs)
    {
        return lhs->getKills() > rhs->getKills();
    });
}

void Leaderboard::updateSlots()
{
    for (auto i = 0; i < leaderboardSlots.size(); i++)
        leaderboardSlots[i]->setHeroStats(heroes[i]);
}
