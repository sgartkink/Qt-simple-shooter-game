#ifndef SNIPER_H
#define SNIPER_H

#include "weapons/gun.h"

// few heavy shots far: one every 1.5 s, a faster bullet; right click aims through its scope (see MapWidget)
class Sniper : public Gun
{
public:
    Sniper();
};

#endif // SNIPER_H
