#ifndef GRENADEELLIPSEITEM_H
#define GRENADEELLIPSEITEM_H

#include <QObject>
#include <QGraphicsEllipseItem>

class GrenadeEllipseItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF rect READ rect WRITE setRect)

public:
    GrenadeEllipseItem();
};

#endif // GRENADEELLIPSEITEM_H
