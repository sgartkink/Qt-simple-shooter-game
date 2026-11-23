#ifndef SHAKESCREEN_H
#define SHAKESCREEN_H

#include <QObject>
#include "core/gameclock.h"
#include <QtMath>
class MapView;

class ShakeScreen : public QObject
{
    Q_OBJECT
public:
    explicit ShakeScreen(MapView * mapView);

    void start();

private:
    MapView * mapView = nullptr;
    GameTimer shakeTimer;
    int mapViewHeight;
    int mapViewWidth;

    double vibration = 0.5;

    void stopShaking();

private slots:
    void shake();
};

#endif // SHAKESCREEN_H
