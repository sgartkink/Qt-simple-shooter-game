#include "leaderboardslot.h"
#include "leaderboard.h"
#include <QtDebug>
LeaderboardSlot::LeaderboardSlot(Leaderboard *lb, HeroStats *hS)
    : gridLayout(lb->getLayout())
{

    int row = gridLayout->rowCount();

    QLabel * lPlace = new QLabel(QString::number(row));
    gridLayout->addWidget(lPlace, row, 0);
    gridLayout->addWidget(lName, row, 1);
    gridLayout->addWidget(lKills, row, 2);
    gridLayout->addWidget(lDeaths, row, 3);

    if (hS)
        setHeroStats(hS);

    connect(this, SIGNAL(changed()), lb, SLOT(update()));
}

void LeaderboardSlot::setHeroStats(HeroStats *hS)
{
    if (heroStats != hS)
    {
        heroStats = hS;
        heroStats->setSlot(this);
        lName->setText(heroStats->getName());
        lKills->setText(QString::number(heroStats->getKills()));
        lDeaths->setText(QString::number(heroStats->getDeaths()));
    }
}

void LeaderboardSlot::updateKills()
{
    lKills->setText(QString::number(heroStats->getKills()));

    emit changed();
}

void LeaderboardSlot::updateDeaths()
{
    lDeaths->setText(QString::number(heroStats->getDeaths()));

    emit changed();
}
