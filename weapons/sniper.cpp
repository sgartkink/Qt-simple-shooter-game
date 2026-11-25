#include "weapons/sniper.h"

Sniper::Sniper()
    : Gun(false, 1500)
{
    damage = 12;            // armour 10 + HP 10: two hits, or one and a few rifle bullets
    capacityAmmo = 5;
    currentLoadedAmmo = capacityAmmo;
    maxAmmo = 20;
    currentOwnedAmmo = maxAmmo;
    bulletSpeed = 10;
}
