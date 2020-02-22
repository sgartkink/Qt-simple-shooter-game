#ifndef GRENADE_H
#define GRENADE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QObject>
#include <QTimer>

#include "itemsonscene.h"
#include "grenadeellipseitem.h"

class Grenade : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    Grenade(double dx_, double dy_, double angle, double addedVelocity, QGraphicsScene *mainScene = nullptr);

private:
    const double radius = 40;

    double velocity = 40;
    double currentHeight = 10;
    int damage = 2;
    int size = 4;

    GrenadeEllipseItem * ellipse = new GrenadeEllipseItem();
    QGraphicsScene * mainScene;

    QDateTime dateTimeStart;
    QTimer moveTimer;
    double currentVelocity;
    double dx;
    double dy;

    void setNewSize(int size_);
    void checkCorner(ItemsOnScene * corner);
    void stopMoving();

private slots:
    void move();
    void explode();
};

#endif // GRENADE_H
