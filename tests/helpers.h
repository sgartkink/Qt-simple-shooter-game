#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QGraphicsScene>

#include "world/mapwidget.h"
#include "world/mapview.h"
#include "ui/playerbarwidget.h"
#include "ui/leaderboard.h"
#include "ui/powergrenadethrowing.h"
#include "ui/messageattopofscreen.h"
#include "characters/player.h"
#include "characters/bot.h"
#include "weapons/bullet.h"
#include "weapons/grenade.h"

// keeps the event loop running, so the game's timers (movement, bullets) fire
inline void runFor(int ms)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < ms)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 5);
}

inline int chestsIn(QGraphicsScene * scene)
{
    int chests = 0;
    for (QGraphicsItem * item : scene->items())
        if (dynamic_cast<Chest *>(item))
            chests++;
    return chests;
}

// what the game sets up around a map: the player, a leaderboard and the overlays, at the real map size.
// MapWidget cleans up the bots, bullets and grenades left on its scene.
struct World
{
    PlayerBarWidget playerBar;
    MapWidget mapWidget {&playerBar};
    Leaderboard leaderboard;
    MessageAtTopOfScreen message;
    Player * mainPlayer = nullptr;

    World()
    {
        // the player MapWidget creates; remembered now, because tests may add more Player items later
        for (QGraphicsItem * item : scene()->items())
            if (Player * p = dynamic_cast<Player *>(item))
                mainPlayer = p;
        mapWidget.setLeaderboard(&leaderboard);
        mapWidget.setMessageAtTopOfScreen(&message);
        mapWidget.setPowerGrenadeThrowing(new PowerGrenadeThrowing);   // MapWidget deletes it
        mapWidget.resize(1024, 580);
        mapWidget.show();
        QCoreApplication::processEvents();
        leaderboard.addNewHero(player()->getHeroStats());
    }

    QGraphicsScene * scene() { return mapWidget.getScene(); }
    Map * map() { return mapWidget.getMap(); }
    MapView * view() { return mapWidget.findChild<MapView *>(); }

    Player * player() { return mainPlayer; }

    Bot * addBot(QPointF pos)
    {
        Bot * bot = new Bot(&mapWidget);
        bot->setPos(pos);
        scene()->addItem(bot);
        leaderboard.addNewHero(bot->getHeroStats());
        return bot;
    }
};

#endif // TEST_HELPERS_H
