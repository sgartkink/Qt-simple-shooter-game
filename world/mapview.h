#ifndef MAPVIEW_H
#define MAPVIEW_H

class ShakeScreen;
#include <QGraphicsView>
#include <QImage>
#include <QMouseEvent>
#include <QVector>
#include "world/map.h"
#include "world/fieldofview.h"

class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    MapView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void startShakeScreen();
    void setVisibleArea(const QVector<VisiblePiece> & visible);

protected:
    void drawForeground(QPainter * painter, const QRectF & rect) override;

private:
    ShakeScreen * shakeScreen;
    QVector<VisiblePiece> visibleArea;
    QImage fogCache;              // darkness for the whole screen, rebuilt only when the view or the camera changes
    QRect fogCacheArea;
    bool fogDirty = true;

    void rebuildFog(const QRect & area);

    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void sendCord(QPoint mousePoint);
    void mousePressed();
    void mouseReleased();
};

#endif // MAPVIEW_H
