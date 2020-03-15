#ifndef BFS_ALGORITHM_H
#define BFS_ALGORITHM_H

#include <QPointF>
#include <QVector>
#include <QtMath>

#include "map.h"
#include "gridelement.h"

class BFS_Algorithm
{
public:
    BFS_Algorithm(Map * map = nullptr);

    QVector<GridElement *> getPath(QPointF source, QPoint destination);

private:
    Map * map = nullptr;
    int distanceFromSource = 0;
    QVector<GridElement *>::iterator it;

    QVector<GridElement *> currentChecking;
    QVector<GridElement *> checkLater;
    QVector<GridElement *> checked;
    QVector<GridElement *> path;

    void iterateThroughMapUntilDestinationFound(QPoint & destination);
    bool checkIfPointEqualsDestination(QPoint & destination);
    void pointEqualsDestination();
    void checkIfLastElement();
    void lastElement();
    void checkNeighbours();
    void neighbour(int x, int y);
    void reset();
};

#endif // BFS_ALGORITHM_H
