#include "bfs_algorithm.h"
#include <QtDebug>

BFS_Algorithm::BFS_Algorithm(Map *map)
    : map(map)
{

}

QVector<GridElement *> BFS_Algorithm::getPath(QPointF source, QPoint destination)
{
    path.clear();
    currentChecking.append(map->getGridElement(qRound(source.x()/10), qRound(source.y()/10)));

    iterateThroughMapUntilDestinationFound(destination);
    while (distanceFromSource != 0)
        checkNeighbours();

    for (auto it = path.begin(); it != path.end(); ++it)
    {
        (*it)->setX((*it)->x()*10);
        (*it)->setY((*it)->y()*10);
    }

    reset();

    return path;
}

void BFS_Algorithm::iterateThroughMapUntilDestinationFound(QPoint &destination)
{
    for (it = currentChecking.begin(); it != currentChecking.end();)
    {
        if (!(*it)->isTaken() && (*it)->getPosition() == -1)
        {
            (*it)->setPosition(distanceFromSource);
            checked << *it;

            if (checkIfPointEqualsDestination(destination))
                break;

            if ((*it)->x() - 1 >= 0)
                checkLater.append(map->getGridElement((*it)->x() - 1, (*it)->y()));
            if ((*it)->x() + 1 <= 248)
                checkLater.append(map->getGridElement((*it)->x() + 1, (*it)->y()));
            if ((*it)->y() - 1 >= 0)
                checkLater.append(map->getGridElement((*it)->x(), (*it)->y() - 1));
            if ((*it)->y() + 1 <= 248)
                checkLater.append(map->getGridElement((*it)->x(), (*it)->y() + 1));
        }

        checkIfLastElement();
    }
}

bool BFS_Algorithm::checkIfPointEqualsDestination(QPoint &destination)
{
    if ((*it)->x() == destination.x()/10 && (*it)->y() == destination.y()/10)
    {
        pointEqualsDestination();
        return true;
    }
    return false;
}

void BFS_Algorithm::pointEqualsDestination()
{
    path << *it;
    distanceFromSource--;
}

void BFS_Algorithm::checkIfLastElement()
{
    if (currentChecking.end() - it == 1)
        lastElement();
    else
        ++it;
}

void BFS_Algorithm::lastElement()
{
    currentChecking = checkLater;
    it = currentChecking.begin();
    checkLater.clear();
    distanceFromSource++;
}

void BFS_Algorithm::checkNeighbours()
{
    if (path.last()->x() - 1 >= 0)
        neighbour(-1, 0);

    if (path.last()->x() + 1 <= 248)
        neighbour(1, 0);

    if (path.last()->y() - 1 >= 0)
        neighbour(0, -1);

    if (path.last()->y() + 1 <= 248)
        neighbour(0, 1);
}

void BFS_Algorithm::neighbour(int x, int y)
{
    if (map->getGridElement(path.last()->x() + x, path.last()->y() + y)->getPosition() == distanceFromSource)
    {
        distanceFromSource--;
        path.append(map->getGridElement(path.last()->x() + x, path.last()->y() + y));
    }
}

void BFS_Algorithm::reset()
{
    distanceFromSource = 0;
    for (auto it = checked.begin(); it != checked.end(); ++it)
        (*it)->setPosition(-1);

    checked.clear();
    currentChecking.clear();
    checkLater.clear();
}
