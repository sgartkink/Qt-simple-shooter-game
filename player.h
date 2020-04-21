#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsView>

#include "hero.h"

class Player : public Hero
{

public:
    Player(QGraphicsView * mapView, MapWidget * mapWidget);

public slots:
    void updateLineHeroMouse(QPoint mousePoint);

protected slots:
    virtual void nextMove();

private:
    const QGraphicsView * mapView;
    ItemsOnScene * corner1 = nullptr;
    ItemsOnScene * corner2 = nullptr;

    QPoint savedMousePoint;
    QPointF mappedPoint;

    void changePosAndUpdateLine(int x_, int y_);
    void checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2, int x_, int y_);
    void newMove(int x_, int y_);

    virtual void death(Bullet * b = nullptr);
};

#endif // PLAYER_H
