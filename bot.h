#ifndef BOT_H
#define BOT_H

#include "hero.h"

class Bot : public Hero
{
public:
    Bot();
    virtual ~Bot();

protected slots:
    virtual void moveHero() {}

public slots:
    virtual void updateLineHeroMouse(QPoint mousePoint) {}
};

#endif // BOT_H
