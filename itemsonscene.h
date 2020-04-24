#ifndef ITEMSONSCENE_H
#define ITEMSONSCENE_H

#include <QGraphicsRectItem>
#include <QPen>

#include "bullet.h"

class ItemsOnScene : public QGraphicsRectItem
{
public:
    ItemsOnScene(Qt::GlobalColor color, qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = nullptr);
    virtual ~ItemsOnScene();

    virtual void attackItem(int damage = 0, Hero * owner = nullptr) = 0;
    double getHeight() { return height; }

protected:
    double height;

    void changeColor(Qt::GlobalColor color);

private:
    QPen pen;
};

#endif // ITEMSONSCENE_H
