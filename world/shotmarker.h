#ifndef SHOTMARKER_H
#define SHOTMARKER_H

#include <QPointF>
#include <QRectF>

class QPainter;

// A red arc close around the hero, pointing to where an enemy last fired from, whether the shot hits or not.
// Every shot replaces the previous one: the arc blinks, holds for HOLD_MS, then fades out over FADE_MS.
// It is always solid red with a dark edge, so it shows on the light ground and in the dark alike. Shots from further
// away give a thinner arc, and ones from beyond MAX_SCREEN_SHARE of the way to the screen edge in that direction
// aren't marked at all.
class ShotMarker
{
public:
    static constexpr int BLINK_MS = 100;
    static constexpr int HOLD_MS = 500;
    static constexpr int FADE_MS = 1500;
    static constexpr qreal MAX_SCREEN_SHARE = 0.75;
    static constexpr qreal MIN_STRENGTH = 0.5;     // just inside the limit
    static constexpr qreal RADIUS = 14;
    static constexpr qreal ARC = 70;               // degrees
    static constexpr qreal WIDTH = 5;              // at full strength; MIN_STRENGTH times that at the limit, +2 while blinking

    // how strongly a shot from `origin` is marked (0 = not at all), for a hero at `hero` seeing `screen` (scene coordinates)
    static qreal strengthFor(QPointF hero, QPointF origin, const QRectF & screen);

    // returns whether the shot is marked
    bool mark(QPointF hero, QPointF origin, const QRectF & screen);
    void clear() { strength = 0; }

    qreal opacityAt(qint64 msSinceShot) const;     // 1 while held, then down to 0 (0 when nothing is marked)
    bool isActiveAt(qint64 msSinceShot) const { return opacityAt(msSinceShot) > 0; }
    QPointF getOrigin() const { return origin; }
    qreal getStrength() const { return strength; }

    void paint(QPainter * painter, QPointF hero, qint64 msSinceShot) const;
    static QRectF area(QPointF hero);              // everything paint() may touch

private:
    QPointF origin;
    qreal strength = 0;
};

#endif // SHOTMARKER_H
