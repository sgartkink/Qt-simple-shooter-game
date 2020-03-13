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

private slots:
    virtual void updateLineHeroMouse(QPoint mousePoint = QPoint());

private:
    QPointF destinationPoint;
    QPainterPath pathNearestArea;

    ItemsOnScene * corner1 = nullptr;
    ItemsOnScene * corner2 = nullptr;
    Hero * targetAttack = nullptr;
    Map * map;

    QVector<GridElement *> currentChecking;
    QVector<GridElement *> checkLater;
    QVector<GridElement *> checked;
    QVector<GridElement *> path;

    int viewRange = 300;

    bool shouldDirectionUp = false;
    bool shouldDirectionDown = false;
    bool shouldDirectionRight = false;
    bool shouldDirectionLeft = false;
    bool goingOpenChest = false;
    bool pathFromAlgorithm = false;

    bool STATE_ATTACK = false;
    bool STATE_HIDE = false;

    void checkNearestArea();
    void randomNewDestinationPoint();
    void newDestinationPoint(int x1, int y1, int x2, int y2);
    void move();
    bool checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2);
    void checkIfBotMayGo();
    void search();
    void attack();
};

#endif // BOT_H
