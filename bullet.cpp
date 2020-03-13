#include "bullet.h"
#include "itemsonscene.h"
#include "player.h"
#include "bot.h"

Bullet::Bullet(double dx, double dy, double angle, QGraphicsScene *mainScene, int damage, Hero *owner)
    : QGraphicsRectItem(0, 0, 4, 2), mainScene(mainScene), dx(dx), dy(dy), damage(damage), owner(owner)
{
    setTransformOriginPoint(2, 1);
    setRotation(-1 * angle);
    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(move()));
    moveTimer.start(10);
}

void Bullet::move()
{
//    QPropertyAnimation * animation = new QPropertyAnimation(this, "pos");
//    animation->setDuration(20);
//    animation->setStartValue(pos());
//    animation->setEndValue(QPointF(pos().x() + dx, pos().y() + dy));
//    animation->start();
    setPos(x() + dx, y() + dy);

    ItemsOnScene * corner0 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos(), QTransform()));
    ItemsOnScene * corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x() + 5, pos().y(), QTransform()));
    ItemsOnScene * corner2 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x(), pos().y() + 5, QTransform()));
    ItemsOnScene * corner3 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x() + 5, pos().y() + 5, QTransform()));

    if (corner0 && corner0 != owner)
    {
        corner0->attackItem(damage);
        delete this;
    }
    else if (corner1 && corner1 != owner)
    {
        corner1->attackItem(damage);
        delete this;
    }
    else if (corner2 && corner2 != owner)
    {
        corner2->attackItem(damage);
        delete this;
    }
    else if (corner3 && corner3 != owner)
    {
        corner3->attackItem(damage);
        delete this;
    }

    if (x() < 0 || x() > 2000 || y() < 0 || y() > 2000)
        delete this;
}
