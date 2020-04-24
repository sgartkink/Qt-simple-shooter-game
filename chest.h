#ifndef CHEST_H
#define CHEST_H

#include <QRandomGenerator>

#include "itemsonscene.h"
#include "hero.h"
#include "messageattopofscreen.h"

class Chest : public ItemsOnScene
{
public:
    Chest();

    virtual void attackItem([[maybe_unused]]int damage = 0, [[maybe_unused]]Hero * owner = nullptr) {}

    void open(Hero * opener, MessageAtTopOfScreen * messageAtTopOfScreen);
    void open(Hero * opener);

private:
    int content = QRandomGenerator::global()->bounded(5);
};

#endif // CHEST_H
