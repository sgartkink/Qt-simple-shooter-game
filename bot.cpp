#include "bot.h"
#include <QtDebug>
#include "player.h"
#include "chest.h"
#include "mapwidget.h"
#include "global_consts.h"
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
    checkNearestArea();

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
    QPainterPath painterPathNearestArea = QPainterPath();
    painterPathNearestArea.addEllipse(pos(), viewRange, viewRange);

    QList<QGraphicsItem *> listOfItemsInNearestArea = scene->items(painterPathNearestArea);
    for (auto it = listOfItemsInNearestArea.begin(); it != listOfItemsInNearestArea.end(); it++)
    {
        if (Player * p = dynamic_cast<Player*>(*it))
        {
            targetAttack = p;
            dependingOnReloadingSetState();
        }
        if (Chest * c = dynamic_cast<Chest*>(*it))
        {
            if (!goingOpenChest)
            {
                destinationPoint = QPoint(c->pos().x() + 10, c->pos().y() + 5);
                goingOpenChest = true;
            }
        }

        if (goingOpenChest && targetAttack)
            break;
    }
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
    {
        STATE_HIDE = true;
        stopShooting();
    }
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
        {
            STATE_ATTACK = false;
            return true;
        }
    }
    return false;
}

void Bot::move()
{
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
    else if (pathFromAlgorithm && goingOpenChest && numberOfAvailableDirections == 0)
    {
        destinationPoint.setX(destinationPoint.x() + 11);
        findPath();
    }

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
            (goingOpenChest && qFabs(x() - destinationPoint.x()) <= 25 && qFabs(y() - destinationPoint.y()) <= 25))
        botIsCloseToDestinationPoint();
}

void Bot::botIsCloseToDestinationPoint()
{
    if (pathFromAlgorithm)
    {
        resetDirections();

        path.last()->setX(path.last()->x()/10);
        path.last()->setY(path.last()->y()/10);
        path.removeLast();
        if (path.length() != 0)
            destinationPoint = *path.last();
        else
        {
            pathFromAlgorithm = false;
            path.clear();
            randomNewDestinationPoint();
        }
    }
    else if (goingOpenChest)
    {
        QPainterPath findingChestPainterPath;
        findingChestPainterPath.addRect(x() - 20, y() - 20, 50, 50);

        QList<QGraphicsItem *> listOfItemsWithinRange = scene->items(findingChestPainterPath);
        for (auto it = listOfItemsWithinRange.begin(); it != listOfItemsWithinRange.end(); it++)
            if (Chest * c = dynamic_cast<Chest*>(*it))
            {
                c->open(this);
                goingOpenChest = false;
                randomNewDestinationPoint();
                break;
            }
    }
    else
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
    if ((x() < 10 && y() < 10) || (x() > MAP_SIZE_X_FULL && y() < 10)
            || (x() < 10 && y() > MAP_SIZE_Y_FULL) || (x() > MAP_SIZE_X_FULL && y() > MAP_SIZE_Y_FULL))
    {
        STATE_HIDE = false;
        randomNewDestinationPoint();
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
    corner1 = dynamic_cast<ItemsOnScene*>(scene->itemAt(x() + addToX_corner1, y() + addToY_corner1, QTransform()));
    corner2 = dynamic_cast<ItemsOnScene*>(scene->itemAt(x() + addToX_corner2, y() + addToY_corner2, QTransform()));

    if (!corner1 && !corner2)
        return true;
    return false;
}

void Bot::findPath()
{
    path = bfs_Algorithm.getPath(pos(), destinationPoint);

    if (path.length() != 0)
    {
        destinationPoint = *path.last();
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

void Bot::updateLineHeroMouse(QPoint mousePoint)
{
    if (STATE_ATTACK)
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
