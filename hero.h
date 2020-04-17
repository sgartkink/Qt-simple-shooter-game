#ifndef HERO_H
#define HERO_H

class MapWidget;
#include <QObject>
#include <QTimer>
#include <QtMath>

#include "shooting.h"
#include "global_consts.h"
#include "itemsonscene.h"
#include "gun.h"
#include "pistol.h"
#include "rifle.h"
#include "shotgun.h"
#include "grenade.h"
#include "herostats.h"

class Hero : public QObject, public ItemsOnScene
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    explicit Hero(MapWidget * mapWidget, Qt::GlobalColor color, qreal x, qreal y, qreal width, qreal height,
                  QGraphicsItem * parent = nullptr);
    ~Hero();

    Shooting * getShooting() { return shooting; }
    HeroStats * getHeroStats() { return &heroStats; }
    qreal lineAngle() { return lineHeroMouse.angle(); }
    Gun * gun() { return currentGun; }
    guns gunENUM(){ return currentGunENUM; }
    int getHP() { return hp; }
    int getGrenades() { return grenades; }
    int getArmor() { return armor; }
    bool isThrowingGrenade() { return throwingGrenade; }
    bool isCurrentlyShooting() { return currentlyShooting; }

    virtual void attackItem(Bullet * b);
    virtual void attackItem(int dmg);
    void heroAttacked(Bullet * b = nullptr);
    void addKill();
    void setCurrentGun(int gun_);
    void addGrenades(int grenades_);
    void addHP(int hp_);
    void addArmor(int armor_);
    void addAmmo(int ammo);
    void refillCurrentLoadedAmmo();
    void refillCurrentOwnedAmmo();
    void throwGrenade(int addedVelocity, MapView * mapView);
    void setCurrentlyShooting(bool c) { currentlyShooting = c; }

    void toggleDirectionUp() { directionUp = !directionUp; }
    void toggleDirectionRight() { directionRight = !directionRight; }
    void toggleDirectionDown() { directionDown = !directionDown; }
    void toggleDirectionLeft() { directionLeft = !directionLeft; }
    void toggleThrowingGrenade() { throwingGrenade = !throwingGrenade; }
    void toggleCurrentlyShooting() { currentlyShooting = !currentlyShooting; }

public slots:
    void start() { moveTimer.start(20); }
    void startShooting();
    void stopShooting();

protected:
    const int size = 10;
    HeroStats heroStats {this};

    int hp = 10;
    int armor = 10;
    int grenades = 3;

    Gun * currentGun;
    Shooting * shooting;
    MapWidget * mapWidget;
    QGraphicsScene * scene;

    guns currentGunENUM;
    Pistol pistol;
    Rifle rifle;
    Shotgun shotgun;
    QLineF lineHeroMouse;
    QTimer shootingTimer;

    bool directionUp = false;
    bool directionRight = false;
    bool directionDown = false;
    bool directionLeft = false;
    bool throwingGrenade = false;
    bool currentlyShooting = false;

protected slots:
    virtual void updateLineHeroMouse(QPoint mousePoint) = 0;
    virtual void nextMove() = 0;

private:
    const int MAX_HP = 15;
    const int MAX_ARMOR = 15;
    const int MAX_GRENADES = 3;
    QTimer moveTimer;
    QPen pen;

    void checkIfStillExist(Bullet * b = nullptr);
    void death(Bullet * b = nullptr);
    void resetHero();
    void randNewPos();
    void changeCurrentGun(Gun &newGun, guns newGunENUM);
    void addValue(int &valueChanged, int &valueAdded, const int valueMax);

signals:
    void posChanged();
    void hpChanged();
    void ammoChangedReloading();
    void ammoChangedNoReloading();
    void grenadesChanged();
    void armorChanged();
    void gunChanged();
};

#endif // HERO_H
