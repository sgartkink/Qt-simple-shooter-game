#include "shotgun.h"

Shotgun::Shotgun()
    : Gun(true, 400)
{
    damage = 2;
    capacityAmmo = 10;
    currentLoadedAmmo = capacityAmmo;
    maxAmmo = 30;
    currentOwnedAmmo = maxAmmo;
}
