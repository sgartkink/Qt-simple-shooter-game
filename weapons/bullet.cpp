#include "weapons/bullet.h"
#include "world/itemsonscene.h"
#include "characters/player.h"
#include "characters/bot.h"
#include "core/global_consts.h"

Bullet::Bullet(Hero *owner, int damage, double dx, double dy)
    : QGraphicsRectItem(0, 0, 4, 2), mainScene(owner->getScene()), dx(dx), dy(dy), damage(damage), owner(owner)
{
    setTransformOriginPoint(2, 1);
    setRotation(-1 * owner->lineAngle());
    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(move()));
    moveTimer.start(10);
}

void Bullet::move()
{
    setPos(x() + dx, y() + dy);

    // the whole rotated bullet against every item it touches, so an explosion or another bullet on top can't hide a target
    for (QGraphicsItem * item : collidingItems())
    {
        ItemsOnScene * target = dynamic_cast<ItemsOnScene *>(item);
        if (!target || target == owner)
            continue;

        if (dynamic_cast<Hero *>(target))
            target->attackItem(damage, owner);
        delete this;
        return;
    }

    if (x() < 0 || x() > MAP_SIZE_X_FULL || y() < 0 || y() > MAP_SIZE_Y_FULL)
        delete this;
}
