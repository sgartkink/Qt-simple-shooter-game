#ifndef BOT_H
#define BOT_H

#include <QPointer>
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

    // grenades: thrown where the target was last seen, or at it while reloading
    QPointer<Grenade> ownGrenade;               // the one in the air or on the ground, until it has gone off
    int grenadeCooldownTicks = 0;
    const int GRENADE_COOLDOWN_TICKS = 300;     // 6 s between grenades
    const qreal GRENADE_MIN_DISTANCE = 100;     // never close enough to catch itself in the blast
    const qreal GRENADE_MAX_DISTANCE = 390;     // lands before the 5 s fuse runs out (a throw with power ~50)
    const qreal GRENADE_DANGER = 70;            // stays this far from a live grenade (blast radius 40)

    // other heroes' grenades it has seen, and where it last saw each: it dodges only what it has noticed
    struct SeenGrenade
    {
        QPointer<Grenade> grenade;
        QPointF at;
    };
    QVector<SeenGrenade> seenGrenades;

    void checkNearestArea();
    bool canSee(QGraphicsItem * item);
    void updateMemory();
    void startLookingAround();
    void dependingOnReloadingSetState();
    bool throwGrenadeAt(QPointF target);
    void noticeGrenade(Grenade * grenade);
    QVector<QPointF> grenadesToAvoid();
    bool avoidGrenades();
    QPointF centre() { return mapToScene(rect().center()); }
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
