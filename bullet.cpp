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

    Building * corner10 = dynamic_cast<Building*>(corner0);
    Building * corner11 = dynamic_cast<Building*>(corner1);
    Building * corner12 = dynamic_cast<Building*>(corner2);
    Building * corner13 = dynamic_cast<Building*>(corner3);

    if (corner0 || corner1 || corner2 || corner3)
    {
        if ((dynamic_cast<Bot*>(corner0) || dynamic_cast<Player*>(corner0)) && corner0 != owner)
        {
            corner0->attackItem(damage);
            delete this;
        }
        else if ((dynamic_cast<Bot*>(corner1) || dynamic_cast<Player*>(corner1)) && corner1 != owner)
        {
            corner1->attackItem(damage);
            delete this;
        }
        else if ((dynamic_cast<Bot*>(corner2) || dynamic_cast<Player*>(corner2)) && corner2 != owner)
        {
            corner2->attackItem(damage);
            delete this;
        }
        else if ((dynamic_cast<Bot*>(corner3) || dynamic_cast<Player*>(corner3)) && corner3 != owner)
        {
            corner3->attackItem(damage);
            delete this;
        }
    }

    if (corner10 || corner11 || corner12 || corner13)
        delete this;

    if (x() < 0 || x() > 2000 || y() < 0 || y() > 2000)
        delete this;
}
