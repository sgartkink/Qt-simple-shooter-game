#include "hero.h"

Hero::Hero(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent, QGraphicsScene * mainScene, Qt::GlobalColor color)
    : ItemsOnScene(x, y, width, height, parent, color), mainScene(mainScene)
{
    setTransformOriginPoint(size/2, size/2);

    currentGun = &rifle;
    currentGunENUM = RIFLE;
    height = 10;

    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(moveHero()));
    moveTimer.start(20);
}

void Hero::attackItem(int dmg)
{
    armor -= dmg;
    emit (armorChanged());

    if (armor < 0)
    {
        hp += armor;
        armor = 0;
        emit (hpChanged());
        emit (armorChanged());
        checkIfStillExist();
    }
}

void Hero::checkIfStillExist()
{
    if (hp <= 0)
        delete this;
}

void Hero::setCurrentGun(int gun_)
{
    switch (gun_)
    {
    case PISTOL:
        changeCurrentGun(pistol, PISTOL);
        break;
    case RIFLE:
        changeCurrentGun(rifle, RIFLE);
        break;
    case SHOTGUN:
        changeCurrentGun(shotgun, SHOTGUN);
        break;
    }
}

void Hero::changeCurrentGun(Gun &newGun, guns newGunENUM)
{
    currentGun = &newGun;
    currentGunENUM = newGunENUM;
    shooting->setGun(currentGun);
}

void Hero::addGrenades(int grenades_)
{
    addValue(grenades, grenades_, MAX_GRENADES);
    emit (grenadesChanged());
}

void Hero::addHP(int hp_)
{
    addValue(hp, hp_, MAX_HP);
    emit (hpChanged());
}

void Hero::addArmor(int armor_)
{
    addValue(armor, armor_, MAX_ARMOR);
    emit (armorChanged());
}

void Hero::addAmmo(int ammo)
{
    currentGun->addAmmo(ammo);
    emit (ammoChangedReloading());
}

void Hero::addValue(int &valueChanged, int &valueAdded, const int valueMax)
{
    if (valueChanged + valueAdded > valueMax)
        valueChanged = valueMax;
    else
        valueChanged += valueAdded;
}

void Hero::refillCurrentLoadedAmmo()
{
    currentGun->refillCurrentOwnedAmmo();
    emit (ammoChangedReloading());
}

void Hero::refillCurrentOwnedAmmo()
{
    currentGun->refillCurrentLoadedAmmo();
    emit (ammoChangedReloading());
}

void Hero::throwGrenade(int addedVelocity)
{
    Grenade * grenade = new Grenade(5*qCos(qDegreesToRadians(lineHeroMouse.angle())),
                                    -5*qSin(qDegreesToRadians(lineHeroMouse.angle())),
                                    lineHeroMouse.angle(),
                                    addedVelocity,
                                    mainScene);
    grenade->setPos(x() + 5, y() + 5);
    mainScene->addItem(grenade);
    grenades--;
    emit (grenadesChanged());
}

void Hero::startShooting()
{
    if (!currentGun->isReloading() && !throwingGrenade)
    {
        currentlyShooting = true;
        shooting->start();
    }
}

void Hero::stopShooting()
{
    currentlyShooting = false;
    shooting->stop();
}
