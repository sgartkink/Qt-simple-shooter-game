#ifndef GRENADE_H
#define GRENADE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QObject>
#include <QTimer>

class MapView;
#include "itemsonscene.h"
#include "grenadeellipseitem.h"

class Grenade : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Grenade(Hero * owner, double addedVelocity, MapView * mapView = nullptr);

private:
    const double radius = 40;

    double velocity = 40;
    double currentHeight = 10;
    int damage = 2;
    int size = 4;
    bool mayMove = true;

    Hero * owner;
    GrenadeEllipseItem * ellipse = new GrenadeEllipseItem();
    QGraphicsItem * previousHitItem = nullptr;
    QGraphicsScene * mainScene = nullptr;
    MapView * mapView = nullptr;
    QList<QGraphicsItem *> corner0;
    QList<QGraphicsItem *> corner1;
    QList<QGraphicsItem *> corner2;

    QDateTime dateTimeStart;
    QTimer moveTimer;
    double currentVelocity;
    double dx;
    double dy;

    void setNewSize(int size_);
    bool checkCorner(ItemsOnScene * corner);
    void setNewRotation();
    void stopMoving();

private slots:
    void move();
    void explode();
};

#endif // GRENADE_H
