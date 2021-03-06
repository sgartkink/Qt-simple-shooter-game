﻿#include "gun.h"
#include "global_consts.h"
#include <QtDebug>
Gun::Gun(bool automatic, int fireFrequency)
    : automatic(automatic), fireFrequency(fireFrequency)
{

}

void Gun::reload()
{    
    reloading = true;
    reloaded();
    QTimer::singleShot(RELOADING_TIME, [this](){ reloading = false; });
}

void Gun::addAmmo(int ammo)
{
    if (currentLoadedAmmo + ammo > capacityAmmo)
    {
        currentOwnedAmmo += capacityAmmo - currentLoadedAmmo;
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
