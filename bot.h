#ifndef BOT_H
#define BOT_H

#include <QRandomGenerator>

#include "hero.h"
#include "map.h"
#include "gridelement.h"
#include "bfs_algorithm.h"

class Bot : public Hero
{
public:
    Bot(QGraphicsScene * mainScene, Map * map);
    virtual ~Bot() {}

protected slots:
    virtual void nextMove();

private slots:
    virtual void updateLineHeroMouse(QPoint mousePoint = QPoint());

private:
    QPoint destinationPoint;
    BFS_Algorithm bfs_Algorithm;

    ItemsOnScene * corner1 = nullptr;
    ItemsOnScene * corner2 = nullptr;
    Hero * targetAttack = nullptr;
    Map * map;

    QVector<GridElement *> path;

    int viewRange = 300;
    unsigned int numberOfAvailableDirections = 0;

    bool goingOpenChest = false;
    bool pathFromAlgorithm = false;

    bool STATE_ATTACK = false;
    bool STATE_HIDE = false;

    void checkNearestArea();
    void dependingOnReloadingSetState();
    bool checkIfItemsBetweenBotAndTarget();

    void move();
    void checkIfBotIsCloseToDestinationPoint();
    void botIsCloseToDestinationPoint();

    void randomNewDestinationPoint();
    void randNewDestPointUntilItMeetsRequirements();
    void newDestinationPointBasingOnHideState();
    void newDestinationPointBasingOnTargetPos();
    void newDestinationPoint(int x1, int y1, int x2, int y2);

    void setDirectionBasingOnAlgorithmAndCorners(bool * direction, int addToX_corner1, int addToY_corner1,
                                                 int addToX_corner2, int addToY_corner2);
    bool checkIfCornersAvailable(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2);
    void findPath();
    void resetDirections();
    void attack();
};

#endif // BOT_H
