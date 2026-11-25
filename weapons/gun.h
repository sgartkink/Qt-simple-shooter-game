#ifndef GUN_H
#define GUN_H

#include <QObject>
#include <QTimer>

class Gun : public QObject
{
    Q_OBJECT

public:
    Gun(bool automatic = false, int fireFrequency = 0);

    int getFireFrequency() { return fireFrequency; }
    int getBulletSpeed() const { return bulletSpeed; }     // px per 10 ms step
    // single-shot guns with a fire frequency can't fire again before it has passed (game time)
    bool readyToFire() const;
    void markFired();
    int getDamage() { return damage; }
    int ammoLoaded() { return currentLoadedAmmo; }
    int ammoOwned() { return currentOwnedAmmo; }
    bool isAutomatic() { return automatic; }
    bool isReloading() { return reloading; }
    bool isFull() { return (currentLoadedAmmo == capacityAmmo) ? true : false; }

    void decreaseAmmo() { currentLoadedAmmo--; }
    void reload();
    void refillCurrentLoadedAmmo() { currentLoadedAmmo = capacityAmmo; }
    void refillCurrentOwnedAmmo() { currentOwnedAmmo = maxAmmo; }
    void addAmmo(int ammo);
    void reset();

protected:
    int damage;
    int capacityAmmo;
    int currentLoadedAmmo;
    int maxAmmo;
    int currentOwnedAmmo;
    int bulletSpeed = 5;

private:
    const bool automatic;

    bool reloading = false;

    int fireFrequency;
    qint64 lastShotAt = -1;

    void reloaded();
};

#endif // GUN_H
