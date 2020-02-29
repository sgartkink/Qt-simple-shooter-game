#ifndef MAPVIEW_H
#define MAPVIEW_H

class ShakeScreen;
#include <QGraphicsView>
#include <QMouseEvent>
#include "map.h"

class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    MapView(QGraphicsScene *scene, QWidget *parent = nullptr);
    ~MapView();

    void startShakeScreen();

private:
    Map * map;
    ShakeScreen * shakeScreen;

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void sendCord(QPoint mousePoint);
    void mousePressed();
    void mouseReleased();
};

#endif // MAPVIEW_H
