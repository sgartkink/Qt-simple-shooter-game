#include "mapview.h"
#include "shakescreen.h"

MapView::MapView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setSceneRect(0,0,1024,680);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    shakeScreen = new ShakeScreen(this);
}

void MapView::startShakeScreen()
{
    shakeScreen->start();
}

void MapView::mouseMoveEvent(QMouseEvent *event)
{
    emit (sendCord(event->pos()));
}

void MapView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == 1)
        emit (mousePressed());
}

void MapView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == 1)
        emit (mouseReleased());
}
