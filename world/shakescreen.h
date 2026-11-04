#ifndef SHAKESCREEN_H
#define SHAKESCREEN_H

#include <QObject>
#include <QTimer>
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
    QTimer shakeTimer;
    int mapViewHeight;
    int mapViewWidth;

    double vibration = 0.5;

    void stopShaking();

private slots:
    void shake();
};

#endif // SHAKESCREEN_H
