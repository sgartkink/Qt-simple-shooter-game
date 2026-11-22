#include "weapons/grenade.h"
#include <QtDebug>
#include "characters/player.h"
#include <QtMath>
#include "world/mapview.h"
#include "core/global_consts.h"

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

Grenade::~Grenade()
{
    delete ellipse;     // the explosion circle, also when the grenade goes before it has exploded
}

qreal Grenade::flightDistance(double addedVelocity)
{
    // the height in move() is 10 + k^2 v t - (9.81 k + 9.81/2) t^2 with k = sin(sqrt(3)/2); it lands when that is 0
    const double v = BASE_VELOCITY + addedVelocity;
    const double k = qSin(sqrt(3)/2);
    const double a = 9.81 * k + 9.81 / 2;
    const double t = (k*k*v + sqrt(k*k*k*k*v*v + 4*a*10)) / (2*a);
    return v * t;
}

double Grenade::powerFor(qreal distance)
{
    if (distance < flightDistance(0) || distance > flightDistance(MAX_POWER))
        return -1;
    double low = 0, high = MAX_POWER;           // the distance grows with the power
    for (int i = 0; i < 40; i++)
    {
        const double mid = (low + high) / 2;
        (flightDistance(mid) < distance ? low : high) = mid;
    }
    return (low + high) / 2;
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

    if (bounceBackIfComingDownOnSomething())
        return;

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
        if (item && (*it) != previousHitItem && !cameDownOn.contains(*it) && item->getHeight() > currentHeight)
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
        if (item && (*it) != previousHitItem && !cameDownOn.contains(*it) && item->getHeight() > currentHeight)
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
        if (item && (*it) != previousHitItem && !cameDownOn.contains(*it) && item->getHeight() > currentHeight)
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
    previousHeight = currentHeight;

    if (x() < 0 || x() > MAP_SIZE_X_FULL || y() < 0 || y() > MAP_SIZE_Y_FULL)
        stopMoving();
}

bool Grenade::bounceBackIfComingDownOnSomething()
{
    // flying over something (a roof, a chest, a hero's head) and now sinking below its top: it bounces back the way
    // it came. Hitting something from the side is left to the corner checks in move().
    const QRectF now(x(), y(), size, size);
    const QRectF before = now.translated(-dx, -dy);
    bool landed = false;
    for (QGraphicsItem * other : mainScene->items(now))
    {
        ItemsOnScene * item = dynamic_cast<ItemsOnScene *>(other);
        if (!item || other == previousHitItem || cameDownOn.contains(other) || item->getHeight() <= currentHeight
                || previousHeight < item->getHeight() || !other->sceneBoundingRect().intersects(before))
            continue;
        cameDownOn.insert(other);       // all of them at once, so it doesn't bounce again off the one next to it
        landed = true;
    }
    if (!landed)
        return false;

    dx = -dx;
    dy = -dy;
    setRotation(rotation() + 180);
    setPos(x() + dx, y() + dy);
    previousHeight = currentHeight;
    return true;
}

void Grenade::setNewRotation()
{
    qreal newRotation = -1 * rotation();
    if (newRotation < 90)
        newRotation = 180 - (90 - newRotation)*2;
    else if (newRotation < 180)
        newRotation = 180 - (90 - (180 - newRotation))*2;
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

    owner->announceAttack(pos());
    mapView->startShakeScreen();
    QTimer::singleShot(300, this, [this](){ delete this; });
}
