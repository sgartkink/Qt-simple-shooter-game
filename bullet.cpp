#include "bullet.h"
#include "itemsonscene.h"
#include "player.h"
#include "bot.h"
#include "global_consts.h"

Bullet::Bullet(Hero *owner, int damage, double dx, double dy)
    : QGraphicsRectItem(0, 0, 4, 2), mainScene(owner->getScene()), dx(dx), dy(dy), damage(damage), owner(owner)
{
    dx = 5*qCos(qDegreesToRadians(owner->lineAngle()));
    dy = -5*qSin(qDegreesToRadians(owner->lineAngle()));
    setTransformOriginPoint(2, 1);
    setRotation(-1 * owner->lineAngle());
    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(move()));
    moveTimer.start(10);
}

void Bullet::move()
{
    setPos(x() + dx, y() + dy);

    ItemsOnScene * corner0 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos(), QTransform()));
    ItemsOnScene * corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x() + 5, pos().y(), QTransform()));
    ItemsOnScene * corner2 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x(), pos().y() + 5, QTransform()));
    ItemsOnScene * corner3 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x() + 5, pos().y() + 5, QTransform()));

    if (corner0 && corner0 != owner)
    {
        if (dynamic_cast<Hero *>(corner0))
            corner0->attackItem(damage, owner);
        delete this;
    }
    else if (corner1 && corner1 != owner)
    {
        if (dynamic_cast<Hero *>(corner1))
            corner1->attackItem(damage, owner);
        delete this;
    }
    else if (corner2 && corner2 != owner)
    {
        if (dynamic_cast<Hero *>(corner2))
            corner2->attackItem(damage, owner);
        delete this;
    }
    else if (corner3 && corner3 != owner)
    {
        if (dynamic_cast<Hero *>(corner3))
            corner3->attackItem(damage, owner);
        delete this;
    }

    if (x() < 0 || x() > MAP_SIZE_X_FULL || y() < 0 || y() > MAP_SIZE_Y_FULL)
        delete this;
}
