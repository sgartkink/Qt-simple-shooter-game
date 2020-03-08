#include "map.h"
#include <QtDebug>

Map::Map(QGraphicsScene * scene)
    : scene(scene)
{
    QPen pen;
    pen.setWidth(3);
    line1 = new QGraphicsLineItem(0,0,1320,0);
    line1->setPen(pen);
    line2 = new QGraphicsLineItem(1320,0,1320,1320);
    line2->setPen(pen);
    line3 = new QGraphicsLineItem(1320,1320,0,1320);
    line3->setPen(pen);
    line4 = new QGraphicsLineItem(0,1320,0,0);
    line4->setPen(pen);
    scene->addItem(line1);
    scene->addItem(line2);
    scene->addItem(line3);
    scene->addItem(line4);

    vector.resize(248);
    int i = 0;
    for (auto it = vector.begin(); it != vector.end(); ++it)
    {
        (*it).resize(248);

        int j = 0;
        for (auto it2 = (*it).begin(); it2 != (*it).end(); ++it2)
        {
            (*it2) = new GridElement(i, j);
            j++;
        }
        i++;
    }

    addBuilding(QPoint(40,28));
    addBuilding(QPoint(40,32));
    addBuilding(QPoint(6,56));
    addBuilding(QPoint(47,28));
    addBuilding(QPoint(23,0));
    addBuilding(QPoint(56,54));
    addBuilding(QPoint(52,32));
    addBuilding(QPoint(67,9));
    addBuilding(QPoint(82,79));
    addBuilding(QPoint(98,85));
    addBuilding(QPoint(125,12));
    addBuilding(QPoint(98,85));
    addBuilding(QPoint(100,100));
    addBuilding(QPoint(63,98));
    addBuilding(QPoint(2,60));
    addBuilding(QPoint(35,85));
    addBuilding(QPoint(52,115));
    addBuilding(QPoint(74,96));
    addBuilding(QPoint(89,95));

    addChest(QPoint(15,18));
}

QVector<QPointF> neighbours(GridElement * center)
{
    QVector<QPointF> n;
    if (center->x() - 1 >= 0)
        n.append(QPointF(center->x() - 1, center->y()));
    if (center->x() + 1 <= 248)
        n.append(QPointF(center->x() + 1, center->y()));
    if (center->y() - 1 >= 0)
        n.append(QPointF(center->x(), center->y() - 1));
    if (center->y() + 1 <= 248)
        n.append(QPointF(center->x(), center->y() + 1));

    return n;
}

void Map::addBuilding(QPointF p)
{
    Building * building = new Building();
    building->setPos(p.x() * 10, p.y() * 10);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            vector[p.x()+i][p.y() + j]->toggleTaken();

    scene->addItem(building);
}

void Map::addChest(QPointF p)
{
    Chest * chest = new Chest();
    chest->setPos(p.x() * 10, p.y() * 10);
    vector[p.x()][p.y()]->toggleTaken();
    scene->addItem(chest);
}

Map::~Map()
{
    delete line1;
    delete line2;
    delete line3;
    delete line4;
}
