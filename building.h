#ifndef BUILDING_H
#define BUILDING_H

#include "itemsonscene.h"

class Building : public ItemsOnScene
{
public:
    Building();
    virtual ~Building();

    virtual void attackItem(int dmg) {;}
};

#endif // BUILDING_H
