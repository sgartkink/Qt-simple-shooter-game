#ifndef FIELDOFVIEW_H
#define FIELDOFVIEW_H

#include <QBrush>
#include <QGraphicsScene>
#include <QPolygonF>
#include <QRectF>
#include <QVector>

// an area cut out of the darkness; the brush's alpha says how much (255 = fully visible)
struct VisiblePiece
{
    QPolygonF area;
    QBrush reveal;
    bool antialiased;
};

class FieldOfView
{
public:
    explicit FieldOfView(QGraphicsScene * scene);

    QVector<VisiblePiece> visibleArea(QPointF origin, qreal aimAngle) const;

private:
    const qreal VIEW_ANGLE = 30;
    const qreal PERIPHERAL_ANGLE = 10;
    const qreal FADE_STEP = 0.5;          // width of one slice of the fade
    const qreal CONE_RAY_STEP = 0.25;     // every slice edge and the middle's overlap land exactly on a ray
    const qreal VIEW_DISTANCE = 2000;
    const qreal NEAR_RADIUS = 30;
    const qreal WALL_DEPTH = 3;

    QVector<QRectF> buildings;

    // ray end points in angle order; rayAngles (if given) receives each point's angle
    QPolygonF castRays(QPointF origin, qreal fromAngle, qreal toAngle, qreal maxDistance, qreal step,
                       QVector<qreal> * rayAngles = nullptr) const;
    // distance to the nearest building wall along the ray; hitAxis: 0 = vertical wall, 1 = horizontal wall, -1 = none
    qreal rayDistance(QPointF origin, QPointF direction, qreal maxDistance, int & hitAxis) const;
};

#endif // FIELDOFVIEW_H
