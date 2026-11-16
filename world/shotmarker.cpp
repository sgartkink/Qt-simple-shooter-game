#include "world/shotmarker.h"

#include <QLineF>
#include <QPainter>
#include <QtMath>

#include <limits>

qreal ShotMarker::strengthFor(QPointF hero, QPointF origin, const QRectF &screen)
{
    const QLineF shot(hero, origin);
    const qreal distance = shot.length();
    if (distance < 1)
        return 1;

    // how far the screen reaches from the hero in the shot's direction
    const qreal dx = shot.dx() / distance, dy = shot.dy() / distance;
    qreal toEdge = std::numeric_limits<qreal>::max();
    if (dx > 0) toEdge = qMin(toEdge, (screen.right() - hero.x()) / dx);
    if (dx < 0) toEdge = qMin(toEdge, (screen.left() - hero.x()) / dx);
    if (dy > 0) toEdge = qMin(toEdge, (screen.bottom() - hero.y()) / dy);
    if (dy < 0) toEdge = qMin(toEdge, (screen.top() - hero.y()) / dy);

    const qreal limit = MAX_SCREEN_SHARE * qMax<qreal>(toEdge, 0);
    if (distance > limit)
        return 0;
    return 1 - (1 - MIN_STRENGTH) * distance / limit;
}

bool ShotMarker::mark(QPointF hero, QPointF origin_, const QRectF &screen)
{
    const qreal s = strengthFor(hero, origin_, screen);
    if (s <= 0)
        return false;
    origin = origin_;
    strength = s;
    return true;
}

qreal ShotMarker::opacityAt(qint64 msSinceShot) const
{
    if (strength <= 0 || msSinceShot < 0 || msSinceShot >= HOLD_MS + FADE_MS)
        return 0;
    if (msSinceShot <= HOLD_MS)
        return 1;
    return 1 - qreal(msSinceShot - HOLD_MS) / FADE_MS;
}

void ShotMarker::paint(QPainter *painter, QPointF hero, qint64 msSinceShot) const
{
    const qreal opacity = opacityAt(msSinceShot);
    if (opacity <= 0)
        return;

    // solid red, never paler with distance (red over the light ground turns pink when transparent): distance only
    // makes it thinner. The blink makes it wider and lighter for a moment, so even a shot while it shows is noticed.
    const bool blinking = msSinceShot < BLINK_MS;
    const qreal width = WIDTH * strength + (blinking ? 2 : 0);
    QColor red = blinking ? QColor(255, 40, 40) : QColor(225, 0, 0);
    QColor edge(70, 0, 0);
    red.setAlphaF(opacity);
    edge.setAlphaF(opacity);

    const QRectF circle(hero - QPointF(RADIUS, RADIUS), QSizeF(2 * RADIUS, 2 * RADIUS));
    const qreal angle = QLineF(hero, origin).angle();      // counter-clockwise from 3 o'clock, like drawArc
    const int start = qRound((angle - ARC / 2) * 16), span = qRound(ARC * 16);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::NoBrush);
    QPen pen(edge, width + 2, Qt::SolidLine, Qt::FlatCap);
    painter->setPen(pen);
    painter->drawArc(circle, start - 16, span + 32);
    pen.setColor(red);
    pen.setWidthF(width);
    painter->setPen(pen);
    painter->drawArc(circle, start, span);
    painter->restore();
}

QRectF ShotMarker::area(QPointF hero)
{
    const qreal reach = RADIUS + WIDTH / 2 + 4;
    return QRectF(hero - QPointF(reach, reach), QSizeF(2 * reach, 2 * reach));
}
