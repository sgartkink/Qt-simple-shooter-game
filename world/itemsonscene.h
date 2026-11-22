#ifndef ITEMSONSCENE_H
#define ITEMSONSCENE_H

#include <QGraphicsRectItem>
#include <QPen>

#include "weapons/bullet.h"

class ItemsOnScene : public QGraphicsRectItem
{
public:
    ItemsOnScene(Qt::GlobalColor color, qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = nullptr);
    virtual ~ItemsOnScene();

    virtual void attackItem(int damage = 0, Hero * owner = nullptr) = 0;
    double getHeight() { return height; }

protected:
    double height = 0;          // how tall it is for grenades: they bounce off it while flying lower

    void changeColor(Qt::GlobalColor color);

private:
    QPen pen;
};

#endif // ITEMSONSCENE_H
