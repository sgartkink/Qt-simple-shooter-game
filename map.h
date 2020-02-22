#ifndef MAP_H
#define MAP_H

#include <QGraphicsScene>
#include <QGraphicsLineItem>

#include "building.h"
#include "chest.h"

class Map
{
public:
    explicit Map(QGraphicsScene * scene = nullptr);

private:
    const QGraphicsScene * scene;
    QGraphicsLineItem * line1;
    QGraphicsLineItem * line2;
    QGraphicsLineItem * line3;
    QGraphicsLineItem * line4;
};

#endif // MAP_H
