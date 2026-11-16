#ifndef MAPVIEW_H
#define MAPVIEW_H

class ShakeScreen;
#include <QGraphicsView>
#include <QElapsedTimer>
#include <QImage>
#include <QMouseEvent>
#include <QTimer>
#include <QVector>
#include "world/map.h"
#include "world/fieldofview.h"
#include "world/shotmarker.h"

class MapView : public QGraphicsView
{
    Q_OBJECT
public:
    MapView(QGraphicsScene *scene, QWidget *parent = nullptr);

    void startShakeScreen();
    void setVisibleArea(const QVector<VisiblePiece> & visible);

    // marks around `hero` where an enemy fired from, on top of the darkness
    void showShot(const QGraphicsItem * hero, QPointF origin);
    void clearShot();
    const ShotMarker & getShotMarker() const { return shotMarker; }
    bool isShotMarkerShown() const { return markedHero && shotMarker.isActiveAt(shotClock.elapsed()); }

protected:
    void drawForeground(QPainter * painter, const QRectF & rect) override;

private:
    ShakeScreen * shakeScreen;
    QVector<VisiblePiece> visibleArea;
    QImage fogCache;              // darkness for the whole screen, rebuilt only when the view or the camera changes
    QRect fogCacheArea;
    bool fogDirty = true;

    ShotMarker shotMarker;
    const QGraphicsItem * markedHero = nullptr;
    QElapsedTimer shotClock;
    QTimer shotMarkerTimer;        // repaints the marker while it shows: it fades, and follows the hero
    QRect shotMarkerPainted;       // the viewport area it was last drawn in
    const int SHOT_MARKER_FRAME = 16;

    QPointF markedHeroCentre() const { return markedHero->sceneBoundingRect().center(); }
    void updateShotMarker();

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
