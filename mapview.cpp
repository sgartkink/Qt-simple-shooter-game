#include "mapview.h"

MapView::MapView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    map = new Map(scene);
    setSceneRect(0,0,1024,680);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
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

MapView::~MapView()
{
    delete map;
}
