#include "grenade.h"
#include <QtDebug>
#include "player.h"
#include <QtMath>
#include "mapview.h"
#include "global_consts.h"

Grenade::Grenade(Hero *owner, double addedVelocity, MapView *mapView)
    : QGraphicsRectItem(0, 0, 4, 4), owner(owner), mainScene(owner->getScene()), mapView(mapView)
{
    setTransformOriginPoint(2,2);
    setRotation(owner->lineAngle());
    dateTimeStart = QDateTime::currentDateTime();

    velocity += addedVelocity;
    dx = 5*qCos(qDegreesToRadians(owner->lineAngle()))*velocity/500;
    dy = -5*qSin(qDegreesToRadians(owner->lineAngle()))*velocity/500;

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

    if (currentHeight < 0)
        stopMoving();

    QRectF rect1(x()-0.25, y()-0.25, 0.5, 0.5);
    QRectF rect2(x()+size-0.25, y()-0.25, 0.5, 0.5);
    QRectF rect3(x(), y() + size - 0.25, 0.5, 0.5);
    corner0 = mainScene->items(rect1);
    corner1 = mainScene->items(rect2);
    corner2 = mainScene->items(rect3);

    mayMove = true;
    for (auto it = corner0.begin(); it != corner0.end(); it++)
    {
        ItemsOnScene * item = dynamic_cast<ItemsOnScene *>(*it);
        if (item && (*it) != dynamic_cast<Player *>(*it)
                && (*it) != previousHitItem && item->getHeight() > currentHeight)
        {
            mayMove = false;
            previousHitItem = (*it);

            if (dx >= 0 && dy >= 0)
                dy *= -1;
            else if (dx > 0 && dy < 0)
                dy *= -1;
            else if (dx <= 0 && dy <= 0)
            {
                if (dynamic_cast<ItemsOnScene *>(mainScene->itemAt(x() - 2, y(), QTransform())))
                    dx *= -1;
                else
                    dy *= -1;
            }
            else
                dx *= -1;

            break;
        }
    }

    for (auto it = corner1.begin(); it != corner1.end(); it++)
    {
        ItemsOnScene * item = dynamic_cast<ItemsOnScene *>(*it);
        if (item && (*it) != dynamic_cast<Player *>(*it)
                && (*it) != previousHitItem && item->getHeight() > currentHeight)
        {
            mayMove = false;
            previousHitItem = (*it);

            if (dx >= 0 && dy >= 0)
                dx *= -1;
            else if (dx > 0 && dy < 0)
                dx *= -1;
            else if (dx <= 0 && dy <= 0)
                dy *= -1;
            else
                dy *= -1;
            break;
        }
    }

    for (auto it = corner2.begin(); it != corner2.end(); it++)
    {
        ItemsOnScene * item = dynamic_cast<ItemsOnScene *>(*it);
        if (item && (*it) != dynamic_cast<Player *>(*it)
                && (*it) != previousHitItem && item->getHeight() > currentHeight)
            if (dx < 0 && dy > 0)
            {
                dy *= -1;
                previousHitItem = (*it);
                mayMove = false;
            }
    }

    if (mayMove)
        setPos(x() + dx, y() + dy);
    else
        setNewRotation();

    if (x() < 0 || x() > MAP_SIZE_X_FULL || y() < 0 || y() > MAP_SIZE_Y_FULL)
        stopMoving();
}

bool Grenade::checkCorner(ItemsOnScene *corner)
{
    if (currentHeight > corner->getHeight())
        return true;
    else
        return false;
}

void Grenade::setNewRotation()
{
    qreal newRotation = -1 * rotation();
    if (newRotation < 90)
        newRotation = 180 - (90 - newRotation)*2;
    else if (newRotation < 180)
        newRotation = 180 - (90 - 180 - newRotation)*2;
    else if (newRotation < 270)
        newRotation = 180 - (90 - (newRotation - 180))*2;
    else
        newRotation = 180 - (90 - (360 - newRotation))*2;
    setRotation(-1 * newRotation);
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
            h->attackItem(damage, owner);

    mapView->startShakeScreen();
    QTimer::singleShot(300, [this](){ delete ellipse; delete this; });
}
