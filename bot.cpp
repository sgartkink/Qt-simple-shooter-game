#include "bot.h"

Bot::Bot()
    : Hero (0, 0, 10, 10, nullptr, nullptr, Qt::green)
{
    shooting = new Shooting(this, currentGun, mainScene, false);
}

Bot::~Bot()
{

}
