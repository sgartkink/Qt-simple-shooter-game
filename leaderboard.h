#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QVector>
#include <QObject>

#include "herostats.h"
#include "leaderboardslot.h"

class Leaderboard : public QWidget
{
    Q_OBJECT
public:
    Leaderboard(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    QGridLayout * getLayout() { return gridLayout; }

    void addNewHero(HeroStats * hS);

public slots:
    void update();

private:
    QGridLayout * gridLayout = new QGridLayout(this);
    QVector<HeroStats *> heroes;
    QVector<LeaderboardSlot *> leaderboardSlots;

    QLabel lNr {"PLACE"};
    QLabel lPlayer {"PLAYER"};
    QLabel lKills {"KILLS"};
    QLabel lDeaths {"DEATHS"};

    void sortHeroes();
    void updateSlots();
};

#endif // LEADERBOARD_H
