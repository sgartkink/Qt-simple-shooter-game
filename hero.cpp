#include "hero.h"
#include "mapwidget.h"
#include <QtDebug>
Hero::Hero(MapWidget *mapWidget, Qt::GlobalColor color, qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : ItemsOnScene(color, x, y, width, height, parent), mapWidget(mapWidget), scene(mapWidget->getScene()), heroColor(color)
{
    setTransformOriginPoint(size/2, size/2);

    currentGun = &rifle;
    currentGunENUM = RIFLE;
    height = 10;

    changeColor(Qt::white);

    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(nextMove()));
}

void Hero::attackItem(Bullet *b)
{
    if (alive)
    {
        armor -= b->getDamage();
        heroAttacked(b);
    }
}

void Hero::attackItem(int dmg)
{
    if (alive)
    {
        armor -= dmg;
        heroAttacked();
    }
}

void Hero::start()
{
    changeColor(heroColor);
    moveTimer.start(TIME_MOVE);
    alive = true;
}

void Hero::heroAttacked(Bullet *b)
{
    emit (armorChanged());

    if (armor < 0)
    {
        hp += armor;
        armor = 0;
        emit (hpChanged());
        emit (armorChanged());
        checkIfStillExist(b);
    }
}

void Hero::checkIfStillExist(Bullet *b)
{
    if (hp <= 0)
        death(b);
}

void Hero::resetHero()
{
    hp = 10;
    armor = 10;
    grenades = 3;
    alive = false;
    moveTimer.stop();
}

void Hero::randNewPos()
{
    QPoint p = mapWidget->randNewFreePos();
    setPos(p.x() * 10, p.y() * 10);

    emit posChanged();
    emit hpChanged();
    emit grenadesChanged();
    emit armorChanged();
    emit ammoChangedNoReloading();
}

void Hero::addKill()
{
    heroStats.increaseKills();
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
    emit (ammoChangedNoReloading());
}

void Hero::addValue(int &valueChanged, int &valueAdded, const int valueMax)
{
    if (valueChanged + valueAdded > valueMax)
        valueChanged = valueMax;
    else
        valueChanged += valueAdded;
}

void Hero::resetGuns()
{
    for (Gun * g : heroGuns)
        g->reset();
}

void Hero::refillCurrentLoadedAmmo()
{
    currentGun->refillCurrentOwnedAmmo();
    emit (ammoChangedNoReloading());
}

void Hero::refillCurrentOwnedAmmo()
{
    currentGun->refillCurrentLoadedAmmo();
    emit (ammoChangedNoReloading());
}

void Hero::throwGrenade(int addedVelocity, MapView *mapView)
{
    Grenade * grenade = new Grenade(lineHeroMouse, addedVelocity, scene, mapView);
    grenade->setPos(x() + 5, y() + 5);
    scene->addItem(grenade);
    grenades--;
    emit (grenadesChanged());
}

void Hero::startShooting()
{
    if (!currentGun->isReloading() && !throwingGrenade && alive)
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

Hero::~Hero()
{
    delete shooting;
}
