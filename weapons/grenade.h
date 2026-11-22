#ifndef GRENADE_H
#define GRENADE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QObject>
#include <QSet>
#include <QTimer>

class MapView;
#include "world/itemsonscene.h"
#include "weapons/grenadeellipseitem.h"

class Grenade : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    friend class TestBot;

public:
    Grenade(Hero * owner, double addedVelocity, MapView * mapView = nullptr);
    ~Grenade();

    static constexpr double BASE_VELOCITY = 40;   // px/s; the throw's power (0..100) adds to it
    static constexpr double MAX_POWER = 100;
    static constexpr double RADIUS = 40;

    // how far a throw with this power flies on open ground before it lands (it flies at its velocity until its
    // arc reaches the ground; see move())
    static qreal flightDistance(double addedVelocity);
    // the power that throws this far, or -1 if no power from 0 to MAX_POWER does
    static double powerFor(qreal distance);

private:
    const double radius = RADIUS;

    double velocity = BASE_VELOCITY;
    double currentHeight = 10;
    double previousHeight = 10;     // at the last step
    int damage = 2;
    int size = 4;
    bool mayMove = true;

    Hero * owner;
    GrenadeEllipseItem * ellipse = new GrenadeEllipseItem();
    QGraphicsItem * previousHitItem = nullptr;
    QSet<QGraphicsItem *> cameDownOn;       // everything it bounced back from above, e.g. both buildings at a seam
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
    void setNewRotation();
    bool bounceBackIfComingDownOnSomething();
    void stopMoving();

private slots:
    void move();
    void explode();
};

#endif // GRENADE_H
