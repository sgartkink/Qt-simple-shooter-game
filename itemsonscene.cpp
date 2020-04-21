#include "itemsonscene.h"

ItemsOnScene::ItemsOnScene(Qt::GlobalColor color, qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent)
    : QGraphicsRectItem(x, y, width, height, parent), pen(color)
{
    setPen(pen);
}

ItemsOnScene::~ItemsOnScene()
{

}

void ItemsOnScene::changeColor(Qt::GlobalColor color)
{
    pen.setColor(color);
    setPen(pen);
}
