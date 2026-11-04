#include "world/chest.h"

Chest::Chest()
    : ItemsOnScene(Qt::yellow, 0, 0, 20, 10)
{
    height = 5;
}

void Chest::open(Hero *opener, MessageAtTopOfScreen * messageAtTopOfScreen)
{
    QString message;
    switch (content)
    {
    case 0:
        opener->addGrenades(1);
        message = "You received: grenade(1)";
        break;
    case 1:
        opener->refillCurrentOwnedAmmo();
        message = "You received: refill current owned ammo";
        break;
    case 2:
        opener->refillCurrentLoadedAmmo();
        message = "You received: refill current loaded ammo";
        break;
    case 3:
        opener->addAmmo(20);
        message = "You received: ammo(20)";
        break;
    case 4:
        opener->addHP(5);
        message = "You received: hp(5)";
        break;
    case 5:
        opener->addArmor(5);
        message = "You received: armor(5)";
        break;
    }

    if (messageAtTopOfScreen)
        messageAtTopOfScreen->showMessage(message);
}
