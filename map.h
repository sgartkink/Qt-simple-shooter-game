#ifndef MAP_H
#define MAP_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QVector>

#include "gridelement.h"
#include "building.h"
#include "chest.h"

class Map
{
public:
    explicit Map(QGraphicsScene * scene = nullptr);
    ~Map();

    GridElement * getGridElement(int x, int y) { return vector[x][y]; }
    QVector<QPoint> neighbours(GridElement center);
    bool checkIfPointIsTaken(QPoint center) { return vector[center.x()][center.y()]->isTaken() == true ? true : false; }
    void togglePoint(QPoint p) { vector[p.x()][p.y()]->toggleTaken(); }

    void addChest(QPoint p);

private:
    QGraphicsScene * scene;
    QGraphicsLineItem * line1;
    QGraphicsLineItem * line2;
    QGraphicsLineItem * line3;
    QGraphicsLineItem * line4;

    QVector<QVector<GridElement *>> vector;

    void addBuilding(QPoint p);
};

#endif // MAP_H
