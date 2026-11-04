#include "characters/bfs_algorithm.h"
#include "core/global_consts.h"
#include <QtDebug>

BFS_Algorithm::BFS_Algorithm(Map *map)
    : map(map)
{

}

QVector<QPoint> BFS_Algorithm::getPath(QPointF source, QPoint destination)
{
    path.clear();
    currentChecking.append(map->getGridElement(qRound(source.x()/10), qRound(source.y()/10)));

    iterateThroughMapUntilDestinationFound(destination);
    if (!path.isEmpty())
        while (distanceFromSource > 0)
            checkNeighbours();

    QVector<QPoint> pixelPath;
    for (GridElement * g : path)
        pixelPath.append(QPoint(g->x()*10, g->y()*10));

    reset();

    return pixelPath;
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
            if ((*it)->x() + 1 <= MAP_SIZE_X_10)
                checkLater.append(map->getGridElement((*it)->x() + 1, (*it)->y()));
            if ((*it)->y() - 1 >= 0)
                checkLater.append(map->getGridElement((*it)->x(), (*it)->y() - 1));
            if ((*it)->y() + 1 <= MAP_SIZE_Y_10)
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

    if (path.last()->x() + 1 <= MAP_SIZE_X_10)
        neighbour(1, 0);

    if (path.last()->y() - 1 >= 0)
        neighbour(0, -1);

    if (path.last()->y() + 1 <= MAP_SIZE_Y_10)
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
