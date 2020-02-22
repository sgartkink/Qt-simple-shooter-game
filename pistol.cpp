#include "pistol.h"

Pistol::Pistol()
    : Gun(false)
{
    damage = 5;
    capacityAmmo = 12;
    currentLoadedAmmo = capacityAmmo;
    maxAmmo = 96;
    currentOwnedAmmo = maxAmmo;
}
