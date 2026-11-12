#include "world/fieldofview.h"
#include "world/building.h"

#include <QLineF>
#include <QtMath>
#include <algorithm>

FieldOfView::FieldOfView(QGraphicsScene *scene)
{
    // buildings never move, so their rectangles are collected once
    for (QGraphicsItem * item : scene->items())
        if (Building * b = dynamic_cast<Building *>(item))
            buildings.append(QRectF(b->pos(), b->rect().size()));
}

QVector<VisiblePiece> FieldOfView::visibleArea(QPointF origin, qreal aimAngle) const
{
    // separate, possibly overlapping pieces: merging them with QPainterPath::united() breaks on the
    // edges rays share with buildings, so MapView cuts each piece out of the darkness on its own
    const qreal halfView = VIEW_ANGLE/2 + PERIPHERAL_ANGLE;
    QVector<qreal> angles;
    const QPolygonF fan = castRays(origin, aimAngle - halfView, aimAngle + halfView, VIEW_DISTANCE, CONE_RAY_STEP, &angles);

    // the part of the fan between two angles from the aim, closed at the hero
    auto sector = [&](qreal from, qreal to) {
        QPolygonF piece;
        piece << origin;
        for (int i = 0; i < fan.size(); i++)
            if (angles[i] >= aimAngle + from - 1e-9 && angles[i] <= aimAngle + to + 1e-9)
                piece << fan[i];
        return piece;
    };

    // fully visible middle; it reaches half a slice into the fade, so its smoothed edge can't leave a dark seam
    QVector<VisiblePiece> visible {
        {sector(-VIEW_ANGLE/2 - FADE_STEP/2, VIEW_ANGLE/2 + FADE_STEP/2), QBrush(Qt::black), true},
        {castRays(origin, 0, 360, NEAR_RADIUS, 10), QBrush(Qt::black), true}
    };

    // linear fade to dark over the outer PERIPHERAL_ANGLE on each side, as thin solid slices: a QConicalGradient
    // looks the same but costs ~6 ms a frame. Slices aren't antialiased, so neighbours fill each pixel exactly once
    const int slices = qRound(PERIPHERAL_ANGLE / FADE_STEP);
    for (int k = 0; k < slices; k++)
    {
        const qreal inner = VIEW_ANGLE/2 + k*FADE_STEP;
        const QBrush reveal(QColor(0, 0, 0, qRound(255 * (1 - (k + 0.5) / slices))));
        visible.append({sector(inner, inner + FADE_STEP), reveal, false});
        visible.append({sector(-inner - FADE_STEP, -inner), reveal, false});
    }

    return visible;
}

QPolygonF FieldOfView::castRays(QPointF origin, qreal fromAngle, qreal toAngle, qreal maxDistance, qreal step,
                                QVector<qreal> *rayAngles) const
{
    const qreal span = toAngle - fromAngle;

    // computed from the index, not accumulated, so they land exactly on fromAngle + i*step
    QVector<qreal> angles;
    const int steps = qCeil(span / step - 1e-9);
    for (int i = 0; i <= steps; i++)
        angles.append(qMin(fromAngle + i*step, toAngle));

    // extra rays just beside every building corner, so shadow edges are exact
    for (const QRectF & b : buildings)
        for (QPointF corner : {b.topLeft(), b.topRight(), b.bottomLeft(), b.bottomRight()})
        {
            QLineF line(origin, corner);
            if (line.length() > maxDistance)
                continue;

            qreal relative = std::fmod(line.angle() - fromAngle + 720, 360);
            for (qreal offset : {-0.01, 0.0, 0.01})
                if (relative + offset >= 0 && relative + offset <= span)
                    angles.append(fromAngle + relative + offset);
        }

    std::sort(angles.begin(), angles.end());

    QPolygonF points;
    for (qreal a : angles)
    {
        QPointF direction(qCos(qDegreesToRadians(a)), -qSin(qDegreesToRadians(a)));
        int hitAxis = -1;
        qreal distance = rayDistance(origin, direction, maxDistance, hitAxis);

        // continue a few px into the wall, so the part of it the player faces shows up (the same depth
        // however steep the angle, capped for grazing rays); the rest of the building stays dark
        if (hitAxis >= 0)
        {
            qreal along = qAbs(hitAxis == 0 ? direction.x() : direction.y());
            distance += qMin(WALL_DEPTH / qMax(along, 0.001), 4 * WALL_DEPTH);
        }
        points << origin + direction * distance;
    }

    if (rayAngles)
        *rayAngles = angles;
    return points;
}

qreal FieldOfView::rayDistance(QPointF origin, QPointF direction, qreal maxDistance, int &hitAxis) const
{
    qreal nearest = maxDistance;
    hitAxis = -1;

    for (int i = 0; i < buildings.size(); i++)
    {
        const QRectF & r = buildings[i];
        qreal tMin = 0;
        qreal tMax = nearest;
        int enterAxis = -1;
        bool hits = true;

        // slab test: the ray must be inside the building's x range and y range at the same time
        const qreal origins[2] = {origin.x(), origin.y()};
        const qreal directions[2] = {direction.x(), direction.y()};
        const qreal lows[2] = {r.left(), r.top()};
        const qreal highs[2] = {r.right(), r.bottom()};
        for (int axis = 0; axis < 2 && hits; axis++)
        {
            if (qFuzzyIsNull(directions[axis]))
            {
                if (origins[axis] < lows[axis] || origins[axis] > highs[axis])
                    hits = false;
                continue;
            }

            qreal t1 = (lows[axis] - origins[axis]) / directions[axis];
            qreal t2 = (highs[axis] - origins[axis]) / directions[axis];
            if (t1 > t2)
                std::swap(t1, t2);
            if (t1 > tMin)
            {
                tMin = t1;
                enterAxis = axis;
            }
            tMax = qMin(tMax, t2);
            if (tMin > tMax)
                hits = false;
        }

        if (hits && tMin < nearest)
        {
            nearest = tMin;
            hitAxis = enterAxis;
        }
    }

    return nearest;
}
