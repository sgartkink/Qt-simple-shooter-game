#ifndef BOT_H
#define BOT_H

#include <QRandomGenerator>

#include "hero.h"
#include "map.h"
#include "gridelement.h"

class Bot : public Hero
{
public:
    Bot(QGraphicsScene * mainScene, Map * map);
    virtual ~Bot() {}

protected slots:
    virtual void nextMove();

public slots:
    virtual void updateLineHeroMouse(QPoint mousePoint) {}

private:
    enum states {
        STATE_MOVE,
        STATE_ATTACK,
        STATE_HIDE
    };

    QPointF destinationPoint;
    states currentState = STATE_MOVE;
    QPainterPath pathNearestArea;

    ItemsOnScene * corner1 = nullptr;
    ItemsOnScene * corner2 = nullptr;
    Hero * targetAttack = nullptr;
    Map * map;

    QVector<GridElement *> currentChecking;
    QVector<GridElement *> checkLater;
    QVector<GridElement *> checked;
    QVector<GridElement *> path;

    bool shouldDirectionUp = false;
    bool shouldDirectionDown = false;
    bool shouldDirectionRight = false;
    bool shouldDirectionLeft = false;
    bool goingOpenChest = false;
    bool pathFromAlgorithm = false;
    bool starta = true;

//    QVector<GridElement *> aStar(GridElement * player, GridElement * dest);
//    static bool isDestination(int x, int y, QPoint * dest) {
//        if (x == dest->x() && y == dest->y()) {
//            return true;
//        }
//        return false;
//    }
//    bool isValid(QPoint p) { return map->checkIfPointIsTaken(p); }
//    QVector<GridElement *> makePath(GridElement * dest);

    void checkNearestArea();
    void randomNewDestinationPoint();
    void updateLineHeroMouse();
    void move();
    bool checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2);
    void checkIfBotMayGo();
    void search();
    void attack();
    void hide();
};

#endif // BOT_H
