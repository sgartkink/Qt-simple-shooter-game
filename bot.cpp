#include "bot.h"
#include <QtDebug>
#include "player.h"
#include "chest.h"
Bot::Bot(QGraphicsScene *mainScene, Map *map)
    : Hero (0, 0, 10, 10, nullptr, mainScene, Qt::green), map(map)
{
    shooting = new Shooting(this, currentGun, mainScene, false);
    currentState = STATE_MOVE;
    randomNewDestinationPoint();
}

void Bot::nextMove()
{
    currentState = STATE_MOVE;
    checkNearestArea();

    switch (currentState)
    {
    case STATE_MOVE:
        stopShooting();
        move();
        break;
    case STATE_ATTACK:
        attack();
        break;
    case STATE_HIDE:
        hide();
        break;
    }
}

void Bot::checkNearestArea()
{
    pathNearestArea = QPainterPath();
    pathNearestArea.addEllipse(pos(), 200, 200);

    QList<QGraphicsItem *> listOfItemsInNearestArea = mainScene->items(pathNearestArea);
    for (auto it = listOfItemsInNearestArea.begin(); it != listOfItemsInNearestArea.end(); it++)
        if (Player * p = dynamic_cast<Player*>(*it))
        {
            if (currentGun->isReloading())
            {
                currentState = STATE_HIDE;
                stopShooting();
            }
            else
            { 
                targetAttack = p;

                QPolygonF polygon;
                polygon << QPointF(x() + 5, y() + 5) << QPointF(p->x(), p->y());

                QPainterPath painterPath;
                painterPath.addPolygon(polygon);

                QList<QGraphicsItem *> list = mainScene->items(painterPath);
                for (auto it = list.begin(); it != list.end(); it++)
                {
                    Building * b = dynamic_cast<Building *>(*it);
                    Chest * c = dynamic_cast<Chest *>(*it);
                    if (b || c)
                    {
                        currentState = STATE_MOVE;
                        return;
                    }
                }
                currentState = STATE_ATTACK;
            }
            break;
        }
        else if (Chest * c = dynamic_cast<Chest*>(*it))
        {
            if (!goingOpenChest)
            {
                currentState = STATE_MOVE;
                destinationPoint = QPointF(c->pos().x() - 10, c->pos().y() - 5);
                destinationPoint = c->pos();
                goingOpenChest = true;
                break;
            }
        }
}

void Bot::updateLineHeroMouse()
{
    if (currentState == STATE_MOVE)
        lineHeroMouse.setLine(x(), y(), destinationPoint.x(), destinationPoint.y());
    else if (currentState == STATE_ATTACK)
        lineHeroMouse.setLine(x(), y(), targetAttack->x(), targetAttack->y());
    setRotation(-1 * lineHeroMouse.angle());
}

void Bot::randomNewDestinationPoint()
{
    directionUp = false;
    directionDown = false;
    directionLeft = false;
    directionRight = false;

    do
    {
        do {
            destinationPoint = QPointF(qRound(x() + QRandomGenerator::global()->bounded(-300,300))/10,
                                       qRound(y() + QRandomGenerator::global()->bounded(-300,300))/10);
        } while(destinationPoint.x() < 0 || destinationPoint.x() > 248 ||
                destinationPoint.y() < 0 || destinationPoint.y() > 248);

    } while(map->checkIfPointIsTaken(destinationPoint));
    destinationPoint.setX(destinationPoint.x()*10);
    destinationPoint.setY(destinationPoint.y()*10);
}

void Bot::move()
{
    if ((qFabs(x() - destinationPoint.x()) <= 2 && qFabs(y() - destinationPoint.y()) <= 2) ||
            (goingOpenChest && qFabs(x() - destinationPoint.x()) <= 25 && qFabs(y() - destinationPoint.y()) <= 25))
    {
        if (pathFromAlgorithm)
        {
            directionUp = false;
            directionDown = false;
            directionLeft = false;
            directionRight = false;

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
            QPainterPath findingChestPath;
            findingChestPath.addRect(x() - 20, y()- 20, 50, 50);
            QList<QGraphicsItem *> listOfItemsWithinRange = mainScene->items(findingChestPath);
            for (auto it = listOfItemsWithinRange.begin(); it != listOfItemsWithinRange.end(); it++)
                if (Chest * c = dynamic_cast<Chest*>(*it))
                {
                    c->open(this);
                    directionUp = false;
                    shouldDirectionUp = false;
                    directionDown = false;
                    shouldDirectionDown = false;
                    directionLeft = false;
                    shouldDirectionLeft = false;
                    directionRight = false;
                    shouldDirectionRight = false;
                    goingOpenChest = false;
                    randomNewDestinationPoint();
                    break;
                }
        }
        else
            randomNewDestinationPoint();
    }

    if (qFabs(x() - destinationPoint.x()) >= 2)
    {
        if (x() < destinationPoint.x())
        {
            if (!pathFromAlgorithm)
            {
                shouldDirectionRight = true;
                if (!checkCorners(size+2, 0, size+2, size))
                    directionRight = true;
                else
                    directionRight = false;
            }
            else
                directionRight = true;
        }
        else
        {
            if (!pathFromAlgorithm)
            {
                shouldDirectionLeft = true;
                if (!checkCorners(-2, 0, -2, size))
                    directionLeft = true;
                else
                    directionLeft = false;
            }
            else
                directionLeft = true;
        }
    }

    if (qFabs(y() - destinationPoint.y()) >= 2)
    {
        if (y() < destinationPoint.y())
        {
            if (!pathFromAlgorithm)
            {
                shouldDirectionDown = true;
                if (!checkCorners(0, size+2, size, size+2))
                    directionDown = true;
                else
                    directionDown = false;
            }
            else
                directionDown = true;
        }
        else
        {
            if (!pathFromAlgorithm)
            {
                shouldDirectionUp = true;
                if (!checkCorners(0, -2, size, -2))
                    directionUp = true;
                else
                    directionUp = false;
            }
            else
                directionUp = true;
        }
    }

    if (!pathFromAlgorithm)
        checkIfBotMayGo();

    shouldDirectionUp = false;
    shouldDirectionDown = false;
    shouldDirectionLeft = false;
    shouldDirectionRight = false;

    if (directionUp)
        setPos(x(), y() - 2);
    if (directionRight)
        setPos(x() + 2, y());
    if (directionDown)
        setPos(x(), y() + 2);
    if (directionLeft)
        setPos(x() - 2, y());

    directionUp = false;
    directionDown = false;
    directionLeft = false;
    directionRight = false;
    updateLineHeroMouse();
}

bool Bot::checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2)
{
    corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(x() + addToX_corner1, y() + addToY_corner1, QTransform()));
    corner2 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(x() + addToX_corner2, y() + addToY_corner2, QTransform()));

    if (!corner1 && !corner2)
        return false;
    return true;
}

void Bot::checkIfBotMayGo()
{
    if (shouldDirectionUp && shouldDirectionLeft && !directionUp && !directionLeft)
        search();
    else if (shouldDirectionUp && shouldDirectionRight && !directionUp && !directionRight)
        search();
    else if (shouldDirectionDown && shouldDirectionLeft && !directionDown && !directionLeft)
        search();
    else if (shouldDirectionDown && shouldDirectionRight && !directionDown && !directionRight)
        search();
    else if (shouldDirectionUp && !directionUp && !directionLeft && !directionRight)
        search();
    else if (shouldDirectionDown && !directionDown && !directionLeft && !directionRight)
        search();
    else if (shouldDirectionLeft && !directionLeft && !directionUp && !directionDown)
        search();
    else if (shouldDirectionRight && !directionRight && !directionUp && !directionDown)
        search();
}

void Bot::search()
{
    currentChecking.append(map->getGridElement(qRound(x()/10), qRound(y()/10)));
    int position = 0;
    bool last = false;
    for (auto it = currentChecking.begin(); it != currentChecking.end();)
    {
        last = false;
        if (!(*it)->isTaken())
        {
            bool gridChecked = false;
            for (auto it2 = checked.begin(); it2 != checked.end(); it2++)
                if (*it == *it2)
                {
                    gridChecked = true;
                    break;
                }

            if (!gridChecked)
            {
                checked.append(map->getGridElement((*it)->x(), (*it)->y()));
                map->getGridElement((*it)->x(), (*it)->y())->setPosition(position);

                if ((*it)->x() == destinationPoint.x()/10 && (*it)->y() == destinationPoint.y()/10)
                    break;

                if ((*it)->x()/10 - 1 >= 0)
                    checkLater.append(map->getGridElement((*it)->x() - 1, (*it)->y()));
                if ((*it)->x()/10 + 1 <= 248)
                    checkLater.append(map->getGridElement((*it)->x() + 1, (*it)->y()));
                if ((*it)->y() - 1 >= 0)
                    checkLater.append(map->getGridElement((*it)->x(), (*it)->y() - 1));
                if ((*it)->y() + 1 <= 248)
                    checkLater.append(map->getGridElement((*it)->x(), (*it)->y() + 1));
            }
        }

        if (it - currentChecking.end() == -1)
        {
            currentChecking = checkLater;
            it = currentChecking.begin();
            checkLater.clear();
            position++;
            last = true;
        }

        if (!last)
            ++it;
    }

    path << checked.last();
    position--;
    while (position != 0)
    {
        if (path.last()->x() - 1 >= 0)
            if (map->getGridElement(path.last()->x() - 1, path.last()->y())->getPosition() == position)
            {
                position--;
                path.append(map->getGridElement(path.last()->x() - 1, path.last()->y()));
            }

        if (path.last()->x() + 1 <= 248)
            if (map->getGridElement(path.last()->x() + 1, path.last()->y())->getPosition() == position)
            {
                position--;
                path.append(map->getGridElement(path.last()->x() + 1, path.last()->y()));
            }

        if (path.last()->y() - 1 >= 0)
            if (map->getGridElement(path.last()->x(), path.last()->y() - 1)->getPosition() == position)
            {
                position--;
                path.append(map->getGridElement(path.last()->x(), path.last()->y() - 1));
            }

        if (path.last()->y() + 1 <= 248)
            if (map->getGridElement(path.last()->x(), path.last()->y() + 1)->getPosition() == position)
            {
                position--;
                path.append(map->getGridElement(path.last()->x(), path.last()->y() + 1));
            }
    }

    for (auto it = checked.begin(); it != checked.end(); ++it)
        (*it)->setPosition(-1);
    checked.clear();
    currentChecking.clear();
    checkLater.clear();

    for (auto it = path.begin(); it != path.end(); ++it)
    {
        (*it)->setX((*it)->x()*10);
        (*it)->setY((*it)->y()*10);
    }

    destinationPoint = *path.last();
    pathFromAlgorithm = true;
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

void Bot::hide()
{

}
