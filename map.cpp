#include "map.h"

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

    Building * building0 = new Building();
    Building * building1 = new Building();
    Building * building2 = new Building();
    Building * building3 = new Building();
    Building * building4 = new Building();
    Building * building5 = new Building();
    Building * building6 = new Building();
    Building * building7 = new Building();
    Building * building8 = new Building();
    Building * building9 = new Building();
    Building * building10 = new Building();
    Building * building11 = new Building();
    Building * building12 = new Building();
    Building * building13 = new Building();
    Building * building14 = new Building();
    Building * building15 = new Building();
    Building * building16 = new Building();
    Building * building17 = new Building();
    Building * building18 = new Building();
    Building * building19 = new Building();
    building0->setPos(450,312);
    building1->setPos(410,312);
    building2->setPos(406,272);
    building3->setPos(63,568);
    building4->setPos(345,76);
    building5->setPos(231,9);
    building6->setPos(568,541);
    building7->setPos(453,453);
    building8->setPos(520,320);
    building9->setPos(678,98);
    building10->setPos(820,793);
    building11->setPos(1256,120);
    building12->setPos(983,855);
    building13->setPos(1000,1000);
    building14->setPos(638,985);
    building15->setPos(25,600);
    building16->setPos(356,852);
    building17->setPos(526,1156);
    building18->setPos(741,963);
    building19->setPos(889,951);
    scene->addItem(building0);
    scene->addItem(building1);
    scene->addItem(building2);
    scene->addItem(building3);
    scene->addItem(building4);
    scene->addItem(building5);
    scene->addItem(building6);
    scene->addItem(building7);
    scene->addItem(building8);
    scene->addItem(building9);
    scene->addItem(building10);
    scene->addItem(building11);
    scene->addItem(building12);
    scene->addItem(building13);
    scene->addItem(building14);
    scene->addItem(building15);
    scene->addItem(building16);
    scene->addItem(building17);
    scene->addItem(building18);
    scene->addItem(building19);

    Chest * chest = new Chest();
    chest->setPos(150,180);
    scene->addItem(chest);
}

Map::~Map()
{
    delete line1;
    delete line2;
    delete line3;
    delete line4;
}
