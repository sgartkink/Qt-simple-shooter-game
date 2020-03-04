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
    checkNearestArea();

//    randomNewDestinationPoint();
//    qDebug() << aStar(map->getGridElement(10,10), map->getGridElement(20,15));

    switch (currentState)
    {
    case STATE_MOVE:
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
//        if (Player * p = dynamic_cast<Player*>(*it))
//        {
//            if (currentGun->isReloading())
//                currentState = STATE_HIDE;
//            else
//            {
//                currentState = STATE_ATTACK;
//                targetAttack = p;
//            }
//            break;
//        }
        /*else */if (Chest * c = dynamic_cast<Chest*>(*it))
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
    if (!goingOpenChest)
    {
        directionUp = false;
        directionDown = false;
        directionLeft = false;
        directionRight = false;

        do
        {
            destinationPoint = QPointF(x() + QRandomGenerator::global()->bounded(-300,300),
                                       y() + QRandomGenerator::global()->bounded(-300,300));
            corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(destinationPoint.x(), destinationPoint.y(), QTransform()));
        } while(destinationPoint.x() < 0 || destinationPoint.x() > 2048 || destinationPoint.y() < 0 || destinationPoint.y() > 2048
                || corner1);
    }
}

void Bot::move()
{
    if (qFabs(x() - destinationPoint.x()) < 35 && qFabs(y() - destinationPoint.y()) < 35)
    {
        if (goingOpenChest)
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
            shouldDirectionRight = true;
            if (!checkCorners(size+2, 0, size+2, size))
                directionRight = true;
            else
                directionRight = false;
        }
        else
        {
            shouldDirectionLeft = true;
            if (!checkCorners(-2, 0, -2, size))
                directionLeft = true;
            else
                directionLeft = false;
        }
    }

    if (qFabs(y() - destinationPoint.y()) >= 2)
    {
        if (y() < destinationPoint.y())
        {
            shouldDirectionDown = true;
            if (!checkCorners(0, size+2, size, size+2))
                directionDown = true;
            else
                directionDown = false;
        }
        else
        {
            shouldDirectionUp = true;
            if (!checkCorners(0, -2, size, -2))
                directionUp = true;
            else
                directionUp = false;
        }
    }

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
        randomNewDestinationPoint();
    else if (shouldDirectionUp && shouldDirectionRight && !directionUp && !directionRight)
        randomNewDestinationPoint();
    else if (shouldDirectionDown && shouldDirectionLeft && !directionDown && !directionLeft)
        randomNewDestinationPoint();
    else if (shouldDirectionDown && shouldDirectionRight && !directionDown && !directionRight)
        randomNewDestinationPoint();
    else if (shouldDirectionUp && !directionUp)
        randomNewDestinationPoint();
    else if (shouldDirectionDown && !directionDown)
        randomNewDestinationPoint();
    else if (shouldDirectionLeft && !directionLeft)
        randomNewDestinationPoint();
    else if (shouldDirectionRight && !directionRight)
        randomNewDestinationPoint();
}

void Bot::attack()
{
    updateLineHeroMouse();
    startShooting();
}

void Bot::hide()
{

}


