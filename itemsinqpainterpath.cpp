#include "itemsinqpainterpath.h"

ItemsInQPainterPath::ItemsInQPainterPath(QGraphicsScene *scene)
    : scene(scene)
{

}

template <typename T>
void ItemsInQPainterPath::test(QPainterPath path, T, void(*senderFunction)())
{
    QList<QGraphicsItem *> list = scene->items(path);
    for (auto it = list.begin(); it != list.end(); it++)
        if (T * t = dynamic_cast<T *>(*it))
            senderFunction();
}
