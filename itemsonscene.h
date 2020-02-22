#ifndef ITEMSONSCENE_H
#define ITEMSONSCENE_H

#include <QGraphicsRectItem>
#include <QPen>

class ItemsOnScene : public QGraphicsRectItem
{
public:
    ItemsOnScene(qreal x, qreal y, qreal width, qreal height, QGraphicsItem * parent = nullptr,
                 Qt::GlobalColor color = Qt::transparent);
    virtual ~ItemsOnScene();

    virtual void attackItem(int dmg) = 0;
    double getHeight() { return height; }

protected:
    double height;

private:
    QPen pen;
};

#endif // ITEMSONSCENE_H
