#include "shakescreen.h"
#include "mapview.h"

ShakeScreen::ShakeScreen(MapView *mapView)
    : mapView(mapView)
{
    mapViewWidth = mapView->width();
    mapViewHeight = mapView->height();
    connect(&shakeTimer, &QTimer::timeout, this, &ShakeScreen::shake);
}

void ShakeScreen::start()
{
    shakeTimer.start(20);
}

void ShakeScreen::shake()
{
    mapView->setSceneRect(mapView->sceneRect().x() + (qSin(vibration)*qCos(vibration))*3,
                          mapView->sceneRect().y() + qCos(vibration)*3,
                          mapView->width(), mapView->height());
    vibration += 0.5;

    if (vibration == 10.0)
        stopShaking();
}

void ShakeScreen::stopShaking()
{
    vibration = 0.5;
    shakeTimer.stop();
}
