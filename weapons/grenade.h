#ifndef GRENADE_H
#define GRENADE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QObject>
#include <QSet>
#include "core/gameclock.h"

class MapView;
#include "world/itemsonscene.h"
#include "weapons/grenadeellipseitem.h"

class Grenade : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    friend class TestBot;
    friend class TestWeapons;

public:
    Grenade(Hero * owner, double addedVelocity, MapView * mapView = nullptr);
    ~Grenade();

    Hero * getOwner() const { return owner; }

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

    Hero * owner;
    GrenadeEllipseItem * ellipse = new GrenadeEllipseItem();
    QSet<QGraphicsItem *> cameDownOn;       // everything it bounced back from above, e.g. both buildings at a seam
    QGraphicsScene * mainScene = nullptr;
    MapView * mapView = nullptr;

    qint64 thrownAt;                // game time
    GameTimer moveTimer;
    double currentVelocity;
    double dx;
    double dy;

    void setNewSize(int size_);
    bool blocks(QGraphicsItem * item) const;
    static bool touches(QGraphicsItem * item, const QRectF & area);
    void faceTheWayItFlies();
    bool bounceBackIfComingDownOnSomething();
    void stopMoving();

private slots:
    void move();
    void explode();
};

#endif // GRENADE_H
