#include "characters/hero.h"
#include "world/mapwidget.h"
#include <QtDebug>
Hero::Hero(MapWidget *mapWidget, Qt::GlobalColor color, qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent)
    : ItemsOnScene(color, x, y, width, height, parent), mapWidget(mapWidget), scene(mapWidget->getScene()), heroColor(color)
{
    setTransformOriginPoint(size/2, size/2);

    currentGun = &rifle;
    currentGunENUM = RIFLE;
    ItemsOnScene::height = 10;      // not the constructor's `height` parameter (the rectangle's), which hides it

    changeColor(Qt::white);

    connect(&moveTimer, &GameTimer::timeout, this, &Hero::nextMove);     // virtual: Player's or Bot's
}

void Hero::attackItem(int damage, Hero *owner)
{
    if (alive)
    {
        armor -= damage;
        heroAttacked(owner);
    }
}

void Hero::announceAttack(QPointF origin)
{
    mapWidget->heroAttacked(this, origin);
}

void Hero::start()
{
    changeColor(heroColor);
    moveTimer.start(TIME_MOVE);
    alive = true;
}

void Hero::heroAttacked(Hero *owner)
{
    emit (armorChanged());

    if (armor < 0)
    {
        hp += armor;
        armor = 0;
        emit (hpChanged());
        emit (armorChanged());
        checkIfStillExist(owner);
    }
}

void Hero::checkIfStillExist(Hero *owner)
{
    if (hp <= 0)
    {
        death();

        if (owner && owner != this)
            owner->addKill();
    }
}

void Hero::resetHero()
{
    hp = 10;
    armor = 10;
    grenades = 3;
    alive = false;
    moveTimer.stop();
    stopShooting();
    resetGuns();
    heroStats.increaseDeath();
    randNewPos();
    changeColor(Qt::white);
    emit (died());

    GameClock::singleShot(TIME_RETURN_TO_LIFE, this, [this]()
    {
        this->start();
    });
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

bool Hero::isBlockedAt(qreal x_, qreal y_)
{
    // check every item at the point, not just the topmost one: an explosion or bullet on top must not hide a wall
    for (QGraphicsItem * item : scene->items(QPointF(x_, y_)))
        if (item != this && dynamic_cast<ItemsOnScene *>(item))
            return true;
    return false;
}

void Hero::resetGuns()
{
    for (Gun * g : heroGuns)
        g->reset();
}

void Hero::refillCurrentLoadedAmmo()
{
    currentGun->refillCurrentLoadedAmmo();
    emit (ammoChangedNoReloading());
}

void Hero::refillCurrentOwnedAmmo()
{
    currentGun->refillCurrentOwnedAmmo();
    emit (ammoChangedNoReloading());
}

Grenade * Hero::throwGrenade(double addedVelocity, MapView *mapView)
{
    Grenade * grenade = new Grenade(this, addedVelocity, mapView);
    grenade->setPos(x() + 5, y() + 5);
    scene->addItem(grenade);
    grenades--;
    emit (grenadesChanged());
    return grenade;
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
