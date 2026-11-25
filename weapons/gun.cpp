#include "weapons/gun.h"
#include "core/global_consts.h"
#include "core/gameclock.h"
#include <QtDebug>
Gun::Gun(bool automatic, int fireFrequency)
    : automatic(automatic), fireFrequency(fireFrequency)
{

}

void Gun::reload()
{    
    reloading = true;
    reloaded();
    GameClock::singleShot(RELOADING_TIME, this, [this](){ reloading = false; });
}

bool Gun::readyToFire() const
{
    return automatic || fireFrequency <= 0 || lastShotAt < 0 || GameClock::now() - lastShotAt >= fireFrequency;
}

void Gun::markFired()
{
    lastShotAt = GameClock::now();
}

void Gun::addAmmo(int ammo)
{
    if (currentLoadedAmmo + ammo > capacityAmmo)
    {
        currentOwnedAmmo = qMin(currentOwnedAmmo + currentLoadedAmmo + ammo - capacityAmmo, maxAmmo);
        currentLoadedAmmo = capacityAmmo;
    }
    else
        currentLoadedAmmo += ammo;
}

void Gun::reset()
{
    currentLoadedAmmo = capacityAmmo;
    currentOwnedAmmo = maxAmmo;
}

void Gun::reloaded()
{
    if (currentOwnedAmmo > 0 && currentLoadedAmmo < capacityAmmo)
    {
        if (capacityAmmo - currentLoadedAmmo < currentOwnedAmmo)
        {
            currentOwnedAmmo -= capacityAmmo - currentLoadedAmmo;
            currentLoadedAmmo = capacityAmmo;
        }
        else
        {
            currentLoadedAmmo += currentOwnedAmmo;
            currentOwnedAmmo = 0;
        }
    }
}
