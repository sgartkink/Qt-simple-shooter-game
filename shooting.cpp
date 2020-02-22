#include "shooting.h"
#include "hero.h"

Shooting::Shooting(Hero *owner, Gun *gun, QGraphicsScene *mainScene, bool mainPlayer)
    : mainPlayer(mainPlayer), owner(owner), gun(gun), mainScene(mainScene)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(shot()));
}

void Shooting::start()
{
    if (gun->ammoLoaded() > 0)
    {
        shot();
        if (gun->isAutomatic())
            timer.start(gun->getFireFrequency());
    }
    else
    {
        gun->reload();

        if (mainPlayer)
            emit (owner->ammoChangedReloading());
    }
}

void Shooting::stop()
{
    timer.stop();
}

void Shooting::shot()
{
    lineAngle = owner->lineAngle();
    Bullet * bullet = new Bullet(5*qCos(qDegreesToRadians(lineAngle)),
                                 -5*qSin(qDegreesToRadians(lineAngle)),
                                 lineAngle,
                                 mainScene,
                                 gun->getDamage());
    bullet->setPos(owner->x() + 5, owner->y() + 5);
    mainScene->addItem(bullet);

    gun->decreaseAmmo();
    if (gun->ammoLoaded() == 0)
        timer.stop();

    if (mainPlayer)
        emit (owner->ammoChangedNoReloading());
}
