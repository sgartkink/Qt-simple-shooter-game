#include "shooting.h"
#include "hero.h"
#include <QtDebug>

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
        owner->stopShooting();

        if (mainPlayer)
            emit (owner->ammoChangedReloading());
    }
}

void Shooting::stop()
{
    timer.stop();
}
#include <QtDebug>
void Shooting::shot()
{
    lineAngle = owner->lineAngle();
    if (owner->gunENUM() == SHOTGUN)
        for (int i = -4; i <= 4; i+=2)
            createBullet(i);
    else
        createBullet();

    gun->decreaseAmmo();
    if (gun->ammoLoaded() == 0)
        timer.stop();

    if (mainPlayer)
        emit (owner->ammoChangedNoReloading());
}

void Shooting::createBullet()
{
    Bullet * bullet = new Bullet(5*qCos(qDegreesToRadians(lineAngle)),
                                 -5*qSin(qDegreesToRadians(lineAngle)),
                                 lineAngle,
                                 mainScene,
                                 gun->getDamage(),
                                 owner);
    bullet->setPos(owner->x() + 5, owner->y() + 5);
    mainScene->addItem(bullet);
}

void Shooting::createBullet(int xy_)
{
    Bullet * bullet = new Bullet(5*qCos(qDegreesToRadians(lineAngle + QRandomGenerator::global()->bounded(-7,7))),
                                 -5*qSin(qDegreesToRadians(lineAngle + QRandomGenerator::global()->bounded(-7,7))),
                                 lineAngle + QRandomGenerator::global()->bounded(-7,7),
                                 mainScene,
                                 gun->getDamage() + QRandomGenerator::global()->bounded(3),
                                 owner);
    bullet->setPos(owner->x() + 5 + xy_, owner->y() + 5 + xy_);
    mainScene->addItem(bullet);
}
