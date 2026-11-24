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
    thrownAt = GameClock::now();

    velocity += addedVelocity;
    dx = 5*qCos(qDegreesToRadians(owner->lineAngle()))*velocity/500;
    dy = -5*qSin(qDegreesToRadians(owner->lineAngle()))*velocity/500;
    faceTheWayItFlies();

    connect(&moveTimer, &GameTimer::timeout, this, &Grenade::move);
    moveTimer.start(10);

    GameClock::singleShot(5000, this, [this](){ explode(); });
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
    double secondsDiff = GameClock::now() - thrownAt;
    secondsDiff /= 1000;
    currentVelocity = velocity * qSin(sqrt(3)/2) - 9.81*secondsDiff;
    currentHeight = 10 + currentVelocity * secondsDiff * qSin(sqrt(3)/2)  - 9.81*secondsDiff*secondsDiff/2;

    if (currentHeight < 0)
        stopMoving();

    if (bounceBackIfComingDownOnSomething())
        return;

    // hitting something from the side: whichever way of the step runs into it is reversed (dx for a side wall, dy
    // for a top or bottom one, both for a corner), and the grenade stays put this step
    const QRectF now(x(), y(), size, size);
    bool reverseX = false, reverseY = false;
    for (QGraphicsItem * other : mainScene->items(now.translated(dx, dy)))
    {
        if (!blocks(other) || touches(other, now))
            continue;               // already over it: that is the coming-down case above, never a side hit
        const bool sideways = touches(other, now.translated(dx, 0));
        const bool upOrDown = touches(other, now.translated(0, dy));
        reverseX |= sideways || !upOrDown;
        reverseY |= upOrDown || !sideways;
    }

    if (reverseX)
        dx = -dx;
    if (reverseY)
        dy = -dy;
    if (reverseX || reverseY)
        faceTheWayItFlies();
    else
        setPos(x() + dx, y() + dy);
    previousHeight = currentHeight;

    if (x() < 0 || x() > MAP_SIZE_X_FULL || y() < 0 || y() > MAP_SIZE_Y_FULL)
        stopMoving();
}

bool Grenade::bounceBackIfComingDownOnSomething()
{
    // flying over something (a roof, a chest, a hero's head) and now sinking below its top: it bounces back the way
    // it came. Hitting something from the side is handled in move().
    const QRectF now(x(), y(), size, size);
    const QRectF before = now.translated(-dx, -dy);
    bool landed = false;
    for (QGraphicsItem * other : mainScene->items(now))
    {
        ItemsOnScene * item = dynamic_cast<ItemsOnScene *>(other);
        if (!item || cameDownOn.contains(other) || item->getHeight() <= currentHeight
                || previousHeight < item->getHeight() || !other->sceneBoundingRect().intersects(before))
            continue;
        cameDownOn.insert(other);       // all of them at once, so it doesn't bounce again off the one next to it
        landed = true;
    }
    if (!landed)
        return false;

    dx = -dx;
    dy = -dy;
    faceTheWayItFlies();
    setPos(x() + dx, y() + dy);
    previousHeight = currentHeight;
    return true;
}

bool Grenade::blocks(QGraphicsItem *item) const
{
    // anything on the map taller than the grenade is flying, except what it has already come down on
    const ItemsOnScene * onMap = dynamic_cast<ItemsOnScene *>(item);
    return onMap && onMap->getHeight() > currentHeight && !cameDownOn.contains(item);
}

bool Grenade::touches(QGraphicsItem *item, const QRectF &area)
{
    // the item's real shape (a hero is turned with its aim), not its bounding box
    QPainterPath path;
    path.addRect(area);
    return item->collidesWithPath(item->mapFromScene(path));
}

void Grenade::faceTheWayItFlies()
{
    setRotation(-QLineF(0, 0, dx, dy).angle());
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
    GameClock::track(animation);

    QPainterPath path;
    path.addEllipse(x() - radius, y() - radius, radius*2, radius*2);

    QList<QGraphicsItem *> listOfItemsWithinExplosionRange = mainScene->items(path);
    for (auto it = listOfItemsWithinExplosionRange.begin(); it != listOfItemsWithinExplosionRange.end(); it++)
        if (Hero * h = dynamic_cast<Hero*>(*it))
            h->attackItem(damage, owner);

    owner->announceAttack(pos());
    mapView->startShakeScreen();
    GameClock::singleShot(300, this, [this](){ delete this; });
}
