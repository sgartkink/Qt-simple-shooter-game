#ifndef BUILDING_H
#define BUILDING_H

#include "itemsonscene.h"

class Building : public ItemsOnScene
{
public:
    Building();
    virtual ~Building();

    virtual void attackItem([[maybe_unused]]int damage = 0, [[maybe_unused]]Hero * owner = nullptr) {}
};

#endif // BUILDING_H
