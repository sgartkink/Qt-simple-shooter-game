#include "characters/bot.h"
#include <QtDebug>
#include "characters/player.h"
#include "world/chest.h"
#include "world/mapwidget.h"
#include "core/global_consts.h"
Bot::Bot(MapWidget *mapWidget)
    : Hero(mapWidget, Qt::green, 0, 0, 10, 10)
{
    shooting = new Shooting(this, currentGun, scene, false);
    bfs_Algorithm = BFS_Algorithm(mapWidget->getMap());
    randomNewDestinationPoint();
    heroStats.setName("bot");
}

void Bot::nextMove()
{
    if (grenadeCooldownTicks > 0)
        grenadeCooldownTicks--;

    checkNearestArea();
    updateMemory();

    // reloading with the target in view: a grenade instead of only hiding
    if (STATE_HIDE && targetAttack)
        throwGrenadeAt(targetAttack->mapToScene(targetAttack->rect().center()));

    move();

    if (STATE_ATTACK)
        attack();
    else if (currentlyShooting)
        stopShooting();

    STATE_HIDE = false;
    STATE_ATTACK = false;
    targetAttack = nullptr;
}

void Bot::checkNearestArea()
{
    // the same view as the player: a screen-sized area around the bot, of which FieldOfView decides what's in sight
    const QSizeF view = mapWidget->viewSize();
    const QRectF area(viewCentre() - QPointF(view.width()/2, view.height()/2), view);

    QList<QGraphicsItem *> listOfItemsInNearestArea = scene->items(area);
    for (auto it = listOfItemsInNearestArea.begin(); it != listOfItemsInNearestArea.end(); it++)
    {
        if (Player * p = dynamic_cast<Player*>(*it))
        {
            if (canSee(p))
            {
                targetAttack = p;
                dependingOnReloadingSetState();
            }
        }
        if (Chest * c = dynamic_cast<Chest*>(*it))
        {
            // while searching for the target, chests can wait
            if (!goingOpenChest && !headingToLastSeen && lookAroundTicks == 0 && canSee(c))
            {
                destinationPoint = freeCellNextToChest(c);
                goingOpenChest = true;
                // a route planned for the old destination would lead the bot away from the chest
                path.clear();
                pathFromAlgorithm = false;
            }
        }

        if (Grenade * g = dynamic_cast<Grenade*>(*it))
        {
            if (g->getOwner() != this && canSee(g))
                noticeGrenade(g);
        }
    }
}

bool Bot::canSee(QGraphicsItem *item)
{
    // noticed if its centre or any corner is in sight
    const QPointF eye = mapToScene(rect().center());
    const QRectF r = item->sceneBoundingRect().adjusted(1, 1, -1, -1);
    for (QPointF point : {r.center(), r.topLeft(), r.topRight(), r.bottomLeft(), r.bottomRight()})
        if (mapWidget->getFieldOfView().canSee(eye, lineHeroMouse.angle(), point))
            return true;
    return false;
}

void Bot::updateMemory()
{
    if (targetAttack)
    {
        // in sight: remember where, and stop any search
        lastSeenTarget = targetAttack->mapToScene(targetAttack->rect().center());
        chasing = true;
        headingToLastSeen = false;
        lookAroundTicks = 0;
    }
    else if (chasing && !headingToLastSeen && lookAroundTicks == 0)
    {
        // just lost sight: go and check the spot where the target was last seen
        destinationPoint = (lastSeenTarget - QPointF(size/2, size/2)).toPoint();
        headingToLastSeen = true;
        goingOpenChest = false;
        path.clear();
        pathFromAlgorithm = false;
        // it is probably just around the corner
        throwGrenadeAt(lastSeenTarget);
    }
}

bool Bot::throwGrenadeAt(QPointF target)
{
    if (!isAlive() || grenades <= 0 || grenadeCooldownTicks > 0 || ownGrenade)
        return false;
    const QLineF throwLine(centre(), target);
    if (throwLine.length() < GRENADE_MIN_DISTANCE || throwLine.length() > GRENADE_MAX_DISTANCE)
        return false;
    const double power = Grenade::powerFor(throwLine.length());
    if (power < 0)
        return false;

    lineHeroMouse.setLine(x(), y(), x() + throwLine.dx(), y() + throwLine.dy());   // the grenade flies the way it faces
    setRotation(-1 * lineHeroMouse.angle());
    ownGrenade = throwGrenade(power, mapWidget->getMapView());
    grenadeCooldownTicks = GRENADE_COOLDOWN_TICKS;
    return true;
}

void Bot::noticeGrenade(Grenade *grenade)
{
    for (SeenGrenade & seen : seenGrenades)
        if (seen.grenade == grenade)
        {
            seen.at = grenade->pos();
            return;
        }
    seenGrenades.append({grenade, grenade->pos()});
}

QVector<QPointF> Bot::grenadesToAvoid()
{
    // its own (it knows where that is), and others' where it last saw them; gone-off ones drop out
    QVector<QPointF> spots;
    if (ownGrenade)
        spots.append(ownGrenade->pos());
    for (int i = seenGrenades.size() - 1; i >= 0; i--)
        if (!seenGrenades[i].grenade)
            seenGrenades.remove(i);
        else
            spots.append(seenGrenades[i].at);
    return spots;
}

bool Bot::avoidGrenades()
{
    // returns true when the bot should stand still this tick
    const QVector<QPointF> spots = grenadesToAvoid();
    if (spots.isEmpty())
        return false;
    auto safe = [&](QPointF point) {
        for (QPointF spot : spots)
            if (QLineF(point, spot).length() < GRENADE_DANGER)
                return false;
        return true;
    };
    // the straight walk from here to `to` keeps clear of every grenade
    auto safeWalk = [&](QPointF to) {
        const QPointF from = centre(), d = to - from;
        const qreal length2 = QPointF::dotProduct(d, d);
        for (QPointF spot : spots)
        {
            const qreal t = length2 > 0 ? qBound<qreal>(0, QPointF::dotProduct(spot - from, d) / length2, 1) : 0;
            if (QLineF(from + t * d, spot).length() < GRENADE_DANGER)
                return false;
        }
        return true;
    };
    const QPointF destination = QPointF(destinationPoint) + QPointF(size/2, size/2);

    QPointF nearest = spots.first();
    for (QPointF spot : spots)
        if (QLineF(centre(), spot).length() < QLineF(centre(), nearest).length())
            nearest = spot;

    if (QLineF(centre(), nearest).length() < GRENADE_DANGER)
    {
        // too close: head away from it, unless already heading away to somewhere safe
        const bool headingAway = QPointF::dotProduct(destination - centre(), centre() - nearest) > 0;
        if (headingAway && safe(destination) && QLineF(destination, nearest).length() >= GRENADE_DANGER + 20)
            return false;
        const QLineF away(nearest, centre());
        for (qreal turn : {0.0, 45.0, -45.0, 90.0, -90.0, 135.0, -135.0})
        {
            QLineF step = away.length() > 0.5 ? away : QLineF(nearest, nearest + QPointF(1, 0));
            step.setAngle(step.angle() + turn);
            step.setLength(GRENADE_DANGER + 40);
            const QPoint cell = ((step.p2() - QPointF(size/2, size/2)) / 10).toPoint();
            if (cell.x() < 0 || cell.x() > MAP_SIZE_X_10 - 1 || cell.y() < 0 || cell.y() > MAP_SIZE_Y_10 - 1
                    || mapWidget->getMap()->checkIfPointIsTaken(cell) || !safe(step.p2()))
                continue;
            destinationPoint = cell * 10;
            path.clear();
            pathFromAlgorithm = false;
            return false;
        }
        return false;
    }

    // heading for a spot near one, or past one: wait until it has gone off
    return !safeWalk(destination);
}

void Bot::startLookingAround()
{
    headingToLastSeen = false;
    lookAroundTicks = LOOK_AROUND_TICKS;
    lookAroundAngle = lineHeroMouse.angle();
    resetDirections();
}

void Bot::dependingOnReloadingSetState()
{
    if (!currentGun->isReloading())
    {
        if (!checkIfItemsBetweenBotAndTarget())
            STATE_ATTACK = true;
        else
            stopShooting();
    }
    else
        STATE_HIDE = true;
}

bool Bot::checkIfItemsBetweenBotAndTarget()
{
    QPolygonF polygon;
    polygon << QPointF(x() + 5, y() + 5) << QPointF(targetAttack->x() + 5, targetAttack->y() + 5);

    QList<QGraphicsItem *> list = scene->items(polygon);
    for (auto it = list.begin(); it != list.end(); it++)
    {
        Building * b = dynamic_cast<Building *>(*it);
        Chest * c = dynamic_cast<Chest *>(*it);
        if (b || c)
            return true;
    }
    return false;
}

void Bot::move()
{
    if (lookAroundTicks > 0)
    {
        // standing where the target was last seen, turning a full circle before giving up
        lookAroundTicks--;
        lookAroundAngle += 360.0 / LOOK_AROUND_TICKS;
        if (lookAroundTicks == 0)
        {
            chasing = false;
            randomNewDestinationPoint();
        }
        updateLineHeroMouse();
        return;
    }

    if (avoidGrenades())
    {
        resetDirections();
        updateLineHeroMouse();
        return;
    }

    checkIfBotIsCloseToDestinationPoint();

    if (qFabs(x() - destinationPoint.x()) >= 2)
    {
        if (x() < destinationPoint.x())
            setDirectionBasingOnAlgorithmAndCorners(&directionRight, size+2, 0, size+2, size);
        else
            setDirectionBasingOnAlgorithmAndCorners(&directionLeft, -2, 0, -2, size);
    }

    if (qFabs(y() - destinationPoint.y()) >= 2)
    {
        if (y() < destinationPoint.y())
            setDirectionBasingOnAlgorithmAndCorners(&directionDown, 0, size+2, size, size+2);
        else
            setDirectionBasingOnAlgorithmAndCorners(&directionUp, 0, -2, size, -2);
    }

    if (!pathFromAlgorithm && numberOfAvailableDirections == 0)
        findPath();

    if (directionUp)
        setPos(x(), y() - 2);
    if (directionRight)
        setPos(x() + 2, y());
    if (directionDown)
        setPos(x(), y() + 2);
    if (directionLeft)
        setPos(x() - 2, y());

    numberOfAvailableDirections = 0;
    resetDirections();
    updateLineHeroMouse();
}

void Bot::checkIfBotIsCloseToDestinationPoint()
{
    if ((qFabs(x() - destinationPoint.x()) <= 2 && qFabs(y() - destinationPoint.y()) <= 2) ||
            // the loose 25 px only for the final stop at the chest, not for waypoints on the way (that cut corners through buildings)
            (goingOpenChest && path.length() <= 1 && qFabs(x() - destinationPoint.x()) <= 25 && qFabs(y() - destinationPoint.y()) <= 25))
        botIsCloseToDestinationPoint();
}

void Bot::botIsCloseToDestinationPoint()
{
    if (pathFromAlgorithm)
    {
        resetDirections();

        path.removeLast();
        if (path.length() != 0)
            destinationPoint = path.last();
        else
        {
            pathFromAlgorithm = false;
            path.clear();
            if (headingToLastSeen)
                startLookingAround();
            else if (goingOpenChest)
                openChestNearby();
            else
                randomNewDestinationPoint();
        }
    }
    else if (headingToLastSeen)
        startLookingAround();
    else if (goingOpenChest)
        openChestNearby();
    else
        randomNewDestinationPoint();
}

QPoint Bot::freeCellNextToChest(Chest * chest)
{
    // the chest's own cells are taken, so pathfinding can't reach them: aim for the nearest free cell beside it
    const int cx = qRound(chest->x()/10);
    const int cy = qRound(chest->y()/10);
    const QPoint candidates[] = { {cx - 1, cy}, {cx + 2, cy}, {cx, cy - 1}, {cx + 1, cy - 1}, {cx, cy + 1}, {cx + 1, cy + 1} };

    QPoint best(-1, -1);
    qreal bestDistance = 0;
    for (const QPoint & c : candidates)
    {
        if (c.x() < 0 || c.x() > MAP_SIZE_X_10 || c.y() < 0 || c.y() > MAP_SIZE_Y_10
                || mapWidget->getMap()->checkIfPointIsTaken(c))
            continue;

        qreal distance = QLineF(pos(), c*10).length();
        if (best.x() < 0 || distance < bestDistance)
        {
            best = c;
            bestDistance = distance;
        }
    }

    if (best.x() < 0)
        return QPoint(chest->x() + 10, chest->y() + 5);
    return best*10;
}

void Bot::openChestNearby()
{
    QPainterPath findingChestPainterPath;
    findingChestPainterPath.addRect(x() - 20, y() - 20, 50, 50);

    QList<QGraphicsItem *> listOfItemsWithinRange = scene->items(findingChestPainterPath);
    for (auto it = listOfItemsWithinRange.begin(); it != listOfItemsWithinRange.end(); it++)
        if (Chest * c = dynamic_cast<Chest*>(*it))
        {
            c->open(this);
            mapWidget->getMap()->removeChest(c);
            break;
        }

    // chest may already be taken by someone else, give up on it either way
    goingOpenChest = false;
    randomNewDestinationPoint();
}

void Bot::randomNewDestinationPoint()
{
    resetDirections();

    randNewDestPointUntilItMeetsRequirements();

    destinationPoint.setX(destinationPoint.x()*10);
    destinationPoint.setY(destinationPoint.y()*10);
}

void Bot::randNewDestPointUntilItMeetsRequirements()
{
    do
    {
        do {
            newDestinationPointBasingOnHideState();
        } while(destinationPoint.x() < 0 || destinationPoint.x() > MAP_SIZE_X_10 ||
                destinationPoint.y() < 0 || destinationPoint.y() > MAP_SIZE_Y_10);
    } while(mapWidget->getMap()->checkIfPointIsTaken(destinationPoint));
}

void Bot::newDestinationPointBasingOnHideState()
{
    if (!STATE_HIDE)
        newDestinationPoint(-viewRange, viewRange, -viewRange, viewRange);
    else
        newDestinationPointBasingOnTargetPos();
}

void Bot::newDestinationPointBasingOnTargetPos()
{
    const bool nearLeft = x() < 10;
    const bool nearRight = x() > MAP_SIZE_X_FULL - size - 10;
    const bool nearTop = y() < 10;
    const bool nearBottom = y() > MAP_SIZE_Y_FULL - size - 10;
    if ((nearLeft || nearRight) && (nearTop || nearBottom))
    {
        STATE_HIDE = false;
        newDestinationPoint(-viewRange, viewRange, -viewRange, viewRange);
        return;
    }

    if (x() < targetAttack->x() && y() < targetAttack->y())
        newDestinationPoint(-viewRange, 0, -viewRange, 0);
    else if(x() < targetAttack->x() && y() > targetAttack->y())
        newDestinationPoint(-viewRange, 0, 0, viewRange);
    else if (x() > targetAttack->x() && y() < targetAttack->y())
        newDestinationPoint(0, viewRange, -viewRange, 0);
    else
        newDestinationPoint(0, viewRange, 0, viewRange);
}

void Bot::newDestinationPoint(int x1, int y1, int x2, int y2)
{
    destinationPoint = QPoint(qRound(x() + QRandomGenerator::global()->bounded(x1,y1))/10,
                              qRound(y() + QRandomGenerator::global()->bounded(x2,y2))/10);
}

void Bot::setDirectionBasingOnAlgorithmAndCorners(bool *direction, int addToX_corner1, int addToY_corner1,
                                                  int addToX_corner2, int addToY_corner2)
{
    if (!pathFromAlgorithm)
    {
        if (checkIfCornersAvailable(addToX_corner1, addToY_corner1, addToX_corner2, addToY_corner2))
        {
            *direction = true;
            numberOfAvailableDirections++;
        }
        else
            *direction = false;
    }
    else
        *direction = true;
}

bool Bot::checkIfCornersAvailable(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2)
{
    return !isBlockedAt(x() + addToX_corner1, y() + addToY_corner1) && !isBlockedAt(x() + addToX_corner2, y() + addToY_corner2);
}

void Bot::findPath()
{
    path = bfs_Algorithm.getPath(pos(), destinationPoint);

    if (path.length() != 0)
    {
        destinationPoint = path.last();
        pathFromAlgorithm = true;
    }
    else
        randomNewDestinationPoint();
}

void Bot::resetDirections()
{
    directionUp = false;
    directionDown = false;
    directionLeft = false;
    directionRight = false;
}

void Bot::updateLineHeroMouse([[maybe_unused]] QPoint mousePoint)
{
    if (lookAroundTicks > 0)
        lineHeroMouse.setLine(x(), y(), x() + 100*qCos(qDegreesToRadians(lookAroundAngle)), y() - 100*qSin(qDegreesToRadians(lookAroundAngle)));
    else if (STATE_ATTACK)
        lineHeroMouse.setLine(x(), y(), targetAttack->x(), targetAttack->y());
    else
        lineHeroMouse.setLine(x(), y(), destinationPoint.x(), destinationPoint.y());
    setRotation(-1 * lineHeroMouse.angle());
}

void Bot::attack()
{
    updateLineHeroMouse();

    if (currentGun->ammoOwned() == 0 && currentGun->ammoLoaded() == 0)
    {
        setCurrentGun(0);

        if (currentGun->ammoOwned() == 0 && currentGun->ammoLoaded() == 0)
        {
            setCurrentGun(1);

            if (currentGun->ammoOwned() == 0 && currentGun->ammoLoaded() == 0)
                setCurrentGun(2);
        }
    }

    if (!currentGun->isReloading() && !currentlyShooting)
        startShooting();
}

void Bot::death()
{
    resetHero();

    path.clear();
    pathFromAlgorithm = false;
    goingOpenChest = false;
    chasing = false;
    headingToLastSeen = false;
    lookAroundTicks = 0;
    grenadeCooldownTicks = 0;
    seenGrenades.clear();
    randomNewDestinationPoint();
}
