#include "chest.h"

Chest::Chest()
    : ItemsOnScene(Qt::yellow, 0, 0, 20, 10)
{
    height = 5;
}

void Chest::open(Hero *opener,  MessageAtTopOfScreen * messageAtTopOfScreen)
{
    switch (content)
    {
    case 0:
        opener->addGrenades(1);
        messageAtTopOfScreen->showMessage("You received: grenade(1)");
        break;
    case 1:
        opener->refillCurrentOwnedAmmo();
        messageAtTopOfScreen->showMessage("You received: refill current owned ammo");
        break;
    case 2:
        opener->refillCurrentLoadedAmmo();
        messageAtTopOfScreen->showMessage("You received: refill current loaded ammo");
        break;
    case 3:
        opener->gun()->addAmmo(20);
        messageAtTopOfScreen->showMessage("You received: ammo(20)");
        break;
    case 4:
        opener->addHP(5);
        messageAtTopOfScreen->showMessage("You received: hp(5)");
        break;
    case 5:
        opener->addArmor(5);
        messageAtTopOfScreen->showMessage("You received: armor(5)");
        break;
    }
    delete this;
}

void Chest::open(Hero *opener)
{
    switch (content)
    {
    case 0:
        opener->addGrenades(1);
        break;
    case 1:
        opener->refillCurrentOwnedAmmo();
        break;
    case 2:
        opener->refillCurrentLoadedAmmo();
        break;
    case 3:
        opener->gun()->addAmmo(20);
        break;
    case 4:
        opener->addHP(5);
        break;
    case 5:
        opener->addArmor(5);
        break;
    }
    delete this;
}
