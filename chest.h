#ifndef CHEST_H
#define CHEST_H

#include <QRandomGenerator>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

#include "itemsonscene.h"
#include "hero.h"

class Chest : public ItemsOnScene
{
public:
    Chest();

    virtual void attackItem(int dmg) {;}

    void open(Hero * opener, QGraphicsScene * scene, double howFarToMoveLabelX, double howFarToMoveLabelY);

private:
    int content = QRandomGenerator::global()->bounded(5);
};

#endif // CHEST_H
