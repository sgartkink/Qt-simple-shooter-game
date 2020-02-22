#ifndef BULLET_H
#define BULLET_H

//#include <QPropertyAnimation>
#include <QObject>
#include <QGraphicsRectItem>
#include <QTimer>

class Bullet : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
//    Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
    Bullet(double dx, double dy, double angle, QGraphicsScene * mainScene, int damage);

private:
    const QGraphicsScene * mainScene;
    const double dx;
    const double dy;
    const int damage;

    QTimer moveTimer;

private slots:
    void move();
};

#endif // BULLET_H
