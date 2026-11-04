#ifndef BULLET_H
#define BULLET_H

class Hero;
#include <QObject>
#include <QGraphicsRectItem>
#include <QTimer>

class Bullet : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    Bullet(Hero * owner, int damage, double dx, double dy);

    int getDamage() { return damage; }
    Hero * getOwner() { return owner; }

private:
    const QGraphicsScene * mainScene;
    double dx;
    double dy;
    const int damage;

    Hero * owner;
    QTimer moveTimer;

private slots:
    void move();
};

#endif // BULLET_H
