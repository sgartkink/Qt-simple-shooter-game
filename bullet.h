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
    Bullet(double dx, double dy, double angle, QGraphicsScene * mainScene, int damage, Hero * owner);

private:
    const QGraphicsScene * mainScene;
    const double dx;
    const double dy;
    const int damage;

    Hero * owner;
    QTimer moveTimer;

private slots:
    void move();
};

#endif // BULLET_H
