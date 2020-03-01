#ifndef BOT_H
#define BOT_H

#include <QRandomGenerator>

#include "hero.h"

class Bot : public Hero
{
public:
    Bot(QGraphicsScene * mainScene, QGraphicsView * mainView = nullptr);
    virtual ~Bot() {}

protected slots:
    virtual void nextMove();

public slots:
    virtual void updateLineHeroMouse(QPoint mousePoint) {}

private:
    enum states {
        MOVE_STATE,
        ATTACK_STATE,
        HIDE_STATE
    };

    QPointF destinationPoint;
    states currentState = MOVE_STATE;

    ItemsOnScene * corner1 = nullptr;
    ItemsOnScene * corner2 = nullptr;

    bool possibleDirectionUp = false;
    bool possibleDirectionDown = false;
    bool possibleDirectionRight = false;
    bool possibleDirectionLeft = false;

    void randomNewDestinationPoint();
    void move();
    bool checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2);
    void attack();
    void hide();
};

#endif // BOT_H
