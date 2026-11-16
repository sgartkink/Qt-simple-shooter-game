#ifndef BOT_H
#define BOT_H

#include <QRandomGenerator>

class Chest;

#include "characters/hero.h"
#include "world/gridelement.h"
#include "characters/bfs_algorithm.h"

class Bot : public Hero
{
    friend class TestBot;

public:
    Bot(MapWidget * mapWidget);
    virtual ~Bot() {}

protected slots:
    virtual void nextMove();

private slots:
    virtual void updateLineHeroMouse(QPoint mousePoint = QPoint());

private:
    QPoint destinationPoint;
    BFS_Algorithm bfs_Algorithm;

    Hero * targetAttack = nullptr;

    QVector<QPoint> path;

    int viewRange = 300;
    unsigned int numberOfAvailableDirections = 0;

    bool goingOpenChest = false;
    bool pathFromAlgorithm = false;

    bool STATE_ATTACK = false;
    bool STATE_HIDE = false;

    // memory: after losing sight of the target, go to where it was last seen and look around there
    bool chasing = false;
    bool headingToLastSeen = false;
    QPointF lastSeenTarget;
    int lookAroundTicks = 0;
    qreal lookAroundAngle = 0;
    const int LOOK_AROUND_TICKS = 50;

    void checkNearestArea();
    bool canSee(QGraphicsItem * item);
    void updateMemory();
    void startLookingAround();
    void dependingOnReloadingSetState();
    bool checkIfItemsBetweenBotAndTarget();

    void move();
    void checkIfBotIsCloseToDestinationPoint();
    void botIsCloseToDestinationPoint();
    void openChestNearby();
    QPoint freeCellNextToChest(Chest * chest);

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

    virtual void death();
};

#endif // BOT_H
