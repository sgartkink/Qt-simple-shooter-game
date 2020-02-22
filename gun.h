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

protected:
    int damage;
    int capacityAmmo;
    int currentLoadedAmmo;
    int maxAmmo;
    int currentOwnedAmmo;

private:
    const bool automatic;

    bool reloading = false;

    QTimer reloadTimer;
    int fireFrequency;

    void reloaded();
};

#endif // GUN_H
