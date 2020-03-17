#ifndef SHOOTING_H
#define SHOOTING_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QRandomGenerator>

class Hero;
#include "gun.h"
#include "bullet.h"

class Shooting : public QObject
{
    Q_OBJECT
public:
    Shooting(Hero * owner, Gun * gun = nullptr, QGraphicsScene * mainScene = nullptr, bool mainPlayer = false);

    void setGun(Gun * gun_) { gun = gun_; }

    void start();
    void stop();

private:
    const bool mainPlayer;
    Hero * owner;
    Gun * gun;
    QGraphicsScene * mainScene;
    QTimer timer;
    qreal lineAngle;

    void createBullet();
    void createBullet(int xy_);

private slots:
    void shot();
};

#endif // SHOOTING_H
