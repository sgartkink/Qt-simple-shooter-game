#include "world/mapview.h"
#include "world/shakescreen.h"

#include <QImage>
#include <QPainter>

MapView::MapView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setSceneRect(0,0,1024,680);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    shakeScreen = new ShakeScreen(this);

    shotMarkerTimer.setInterval(SHOT_MARKER_FRAME);
    connect(&shotMarkerTimer, &QTimer::timeout, this, &MapView::updateShotMarker);
}

void MapView::startShakeScreen()
{
    shakeScreen->start();
}

void MapView::setVisibleArea(const QVector<VisiblePiece> &visible)
{
    visibleArea = visible;
    fogDirty = true;
    viewport()->update();
}

void MapView::drawForeground(QPainter *painter, const QRectF &rect)
{
    // most repaints (a bullet, the bot, the hero turning) only cover a small area and the view hasn't changed,
    // so the darkness is built once for the whole screen and only the exposed part is copied
    // (with a small margin, so rounding never leaves an exposed strip outside the cache)
    const QRect screen = mapToScene(viewport()->rect()).boundingRect().toAlignedRect().adjusted(-2, -2, 2, 2);
    if (fogDirty || fogCacheArea != screen)
        rebuildFog(screen);

    const qreal ratio = fogCache.devicePixelRatio();
    const QRectF source = rect.translated(-fogCacheArea.topLeft());
    painter->drawImage(rect, fogCache, QRectF(source.topLeft() * ratio, source.size() * ratio));

    if (markedHero)
        shotMarker.paint(painter, markedHeroCentre(), shotClock.elapsed());
}

void MapView::showShot(const QGraphicsItem *hero, QPointF origin)
{
    if (!shotMarker.mark(hero->sceneBoundingRect().center(), origin, mapToScene(viewport()->rect()).boundingRect()))
        return;     // too far away to mark; a marker still fading from an earlier shot carries on
    markedHero = hero;
    shotClock.start();
    shotMarkerTimer.start();
    updateShotMarker();
}

void MapView::clearShot()
{
    shotMarker.clear();
    updateShotMarker();
}

void MapView::updateShotMarker()
{
    // the area it was drawn in last time and where it goes now, so it never leaves a trail behind a moving hero
    QRect now;
    if (isShotMarkerShown())
        now = mapFromScene(ShotMarker::area(markedHeroCentre())).boundingRect().adjusted(-1, -1, 1, 1);
    else
    {
        shotMarkerTimer.stop();
        markedHero = nullptr;
    }
    viewport()->update(shotMarkerPainted.united(now));
    shotMarkerPainted = now;
}

void MapView::rebuildFog(const QRect &area)
{
    // darkness over the area, with every visible piece erased from it as strongly as its brush says
    const qreal ratio = devicePixelRatioF();
    fogCache = QImage(area.size() * ratio, QImage::Format_ARGB32_Premultiplied);
    fogCache.setDevicePixelRatio(ratio);
    fogCache.fill(Qt::black);
    fogCacheArea = area;
    fogDirty = false;

    QPainter fogPainter(&fogCache);
    fogPainter.translate(-area.topLeft());
    fogPainter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    fogPainter.setPen(Qt::NoPen);
    for (const VisiblePiece & piece : visibleArea)
    {
        fogPainter.setRenderHint(QPainter::Antialiasing, piece.antialiased);
        fogPainter.setBrush(piece.reveal);
        fogPainter.drawPolygon(piece.area);
    }
    fogPainter.end();
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
