#include "grenade.h"
#include <QtDebug>
#include "player.h"
#include <QtMath>

Grenade::Grenade(double dx_, double dy_, double angle, double addedVelocity, QGraphicsScene *mainScene)
    : QGraphicsRectItem(0, 0, 4, 4), mainScene(mainScene)
{
    setTransformOriginPoint(2,2);
    setRotation(-1 * angle);
    dateTimeStart = QDateTime::currentDateTime();

    velocity += addedVelocity;
    dx = dx_*velocity/500;
    dy = dy_*velocity/500;

    connect(&moveTimer, SIGNAL(timeout()), this, SLOT(move()));
    moveTimer.start(10);

    QTimer::singleShot(5000, this, SLOT(explode()));
}

void Grenade::move()
{
    QDateTime dateTimeCurrent = QDateTime::currentDateTime();
    double secondsDiff = dateTimeStart.msecsTo(dateTimeCurrent);
    secondsDiff /= 1000;
    currentVelocity = velocity * qSin(sqrt(3)/2) - 9.81*secondsDiff;
    currentHeight = 10 + currentVelocity * secondsDiff * qSin(sqrt(3)/2)  - 9.81*secondsDiff*secondsDiff/2;

    if (currentHeight > 0 && currentHeight < 10)
        setNewSize(4);
    else if (currentHeight > 10 && currentHeight < 20)
        setNewSize(5);
    else if (currentHeight > 20 && currentHeight < 30)
        setNewSize(6);
    else if (currentHeight > 30)
        setNewSize(7);
    else if (currentHeight < 0)
        stopMoving();

    ItemsOnScene * corner0 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos(), QTransform()));
    ItemsOnScene * corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x() + size+1, pos().y(), QTransform()));
    ItemsOnScene * corner2 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x(), pos().y() + size+1, QTransform()));
    ItemsOnScene * corner3 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(pos().x() + size+1, pos().y() + size+1, QTransform()));

    Player * corner10 = dynamic_cast<Player*>(corner0);
    Player * corner11 = dynamic_cast<Player*>(corner1);
    Player * corner12 = dynamic_cast<Player*>(corner2);
    Player * corner13 = dynamic_cast<Player*>(corner3);

    if ((corner0 || corner1 || corner2 || corner3) && (!corner10 && !corner11 && !corner12 && !corner13))
    {
        if (corner0)
            checkCorner(corner0);
        else if (corner1)
            checkCorner(corner1);
        else if (corner2)
            checkCorner(corner2);
        else if (corner3)
            checkCorner(corner3);
    }
    else
        setPos(x() + dx, y() + dy);

    if (x() < 0 || x() > 1000 || y() < 0 || y() > 1000)
        stopMoving();
}

void Grenade::setNewSize(int size_)
{
    size = size_;
    setRect(0,0,size,size);
}

void Grenade::checkCorner(ItemsOnScene *corner)
{
    if (currentHeight > corner->getHeight())
        setPos(x() + dx, y() + dy);
    else
        stopMoving();
}

void Grenade::stopMoving()
{
    moveTimer.stop();
}

void Grenade::explode()
{
    moveTimer.stop();

    ellipse->setPos(pos());
    mainScene->addItem(ellipse);

    QPropertyAnimation * animation = new QPropertyAnimation(ellipse, "rect");
    animation->setDuration(300);
    animation->setStartValue(QRectF(0, 0, 0, 0));
    animation->setEndValue(QRectF(-radius, -radius, radius*2, radius*2));
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    QPainterPath path;
    path.addEllipse(x() - radius, y() - radius, radius*2, radius*2);

    QList<QGraphicsItem *> listOfItemsWithinExplosionRange = mainScene->items(path);
    for (auto it = listOfItemsWithinExplosionRange.begin(); it != listOfItemsWithinExplosionRange.end(); it++)
        if (Hero * h = dynamic_cast<Hero*>(*it))
            h->attackItem(damage);

    QTimer::singleShot(300, [this](){ delete ellipse; delete this; });
}
