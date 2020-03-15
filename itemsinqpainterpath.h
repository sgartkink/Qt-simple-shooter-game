#ifndef ITEMSINQPAINTERPATH_H
#define ITEMSINQPAINTERPATH_H

#include <QPainterPath>
#include <QGraphicsScene>

class ItemsInQPainterPath
{
public:
    ItemsInQPainterPath(QGraphicsScene * scene);

    template <typename T>
    void test(QPainterPath path, T, void(*senderFunction)());

private:
    QGraphicsScene * scene;

    typedef void(*senderFunction)();
};

#endif // ITEMSINQPAINTERPATH_H
