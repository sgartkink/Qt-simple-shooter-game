#include "rifle.h"

Rifle::Rifle()
    : Gun(true, 100)
{
    damage = 10;
    capacityAmmo = 30;
    currentLoadedAmmo = capacityAmmo;
    maxAmmo = 210;
    currentOwnedAmmo = maxAmmo;
}
