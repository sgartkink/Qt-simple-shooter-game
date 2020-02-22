#include "itemsonscene.h"

ItemsOnScene::ItemsOnScene(qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent, Qt::GlobalColor color)
    : QGraphicsRectItem (x, y, width, height, parent), pen(color)
{
    setPen(pen);
}

ItemsOnScene::~ItemsOnScene()
{

}
