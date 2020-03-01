#include "bot.h"
#include <QtDebug>
Bot::Bot(QGraphicsScene *mainScene, QGraphicsView *mainView)
    : Hero (0, 0, 10, 10, nullptr, mainScene, Qt::green)
{
    shooting = new Shooting(this, currentGun, mainScene, false);
}

void Bot::nextMove()
{
    switch (currentState)
    {
    case MOVE_STATE:
        move();
        break;
    case ATTACK_STATE:
        attack();
        break;
    case HIDE_STATE:
        hide();
        break;
    }
}

void Bot::randomNewDestinationPoint()
{
    directionUp = false;
    possibleDirectionUp = false;
    directionDown = false;
    possibleDirectionDown = false;
    directionLeft = false;
    possibleDirectionLeft = false;
    directionRight = false;
    possibleDirectionRight = false;

    do
    {
        destinationPoint = QPointF(x() + QRandomGenerator::global()->bounded(-300,300),
                                   y() + QRandomGenerator::global()->bounded(-300,300));
        corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(destinationPoint.x(), destinationPoint.y(), QTransform()));

    } while(destinationPoint.x() < 0 || destinationPoint.x() > 2048 || destinationPoint.y() < 0 || destinationPoint.y() > 2048
            || !corner1);
}

void Bot::move()
{
    if (qFabs(x() - destinationPoint.x()) < 5 || qFabs(y() - destinationPoint.y()) < 5 || destinationPoint.isNull())
        randomNewDestinationPoint();

    if (x() < destinationPoint.x())
    {
        possibleDirectionRight = true;
        if (!checkCorners(size+2, 0, size+2, size))
            directionRight = true;
        else
            directionRight = false;
    }
    else
    {
        possibleDirectionLeft = true;
        if (!checkCorners(-2, 0, -2, size))
            directionLeft = true;
        else
            directionLeft = false;
    }

    if (y() < destinationPoint.y())
    {
        possibleDirectionDown = true;
        if (!checkCorners(0, size+2, size, size+2))
            directionDown = true;
        else
            directionDown = false;
    }
    else
    {
        possibleDirectionUp = true;
        if (!checkCorners(0, -2, size, -2))
            directionUp = true;
        else
            directionUp = false;
    }

    if (possibleDirectionUp && possibleDirectionLeft && !directionUp && !directionLeft)
        randomNewDestinationPoint();
    else if (possibleDirectionUp && possibleDirectionRight && !directionUp && !directionRight)
        randomNewDestinationPoint();
    else if (possibleDirectionDown && possibleDirectionLeft && !directionDown && !directionLeft)
        randomNewDestinationPoint();
    else if (possibleDirectionDown && possibleDirectionRight && !directionDown && !directionRight)
        randomNewDestinationPoint();

    possibleDirectionUp = false;
    possibleDirectionDown = false;
    possibleDirectionLeft = false;
    possibleDirectionRight = false;

    if (directionUp)
        setPos(x(), y() - 2);
    if (directionRight)
        setPos(x() + 2, y());
    if (directionDown)
        setPos(x(), y() + 2);
    if (directionLeft)
        setPos(x() - 2, y());
}

bool Bot::checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2)
{
    corner1 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(x() + addToX_corner1, y() + addToY_corner1, QTransform()));
    corner2 = dynamic_cast<ItemsOnScene*>(mainScene->itemAt(x() + addToX_corner2, y() + addToY_corner2, QTransform()));

    if (!corner1 && !corner2)
        return false;
    return true;
}

void Bot::attack()
{

}

void Bot::hide()
{

}
