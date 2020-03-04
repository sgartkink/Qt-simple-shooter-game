#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>

class MainWidget;
#include "mapview.h"
#include "bot.h"
#include "player.h"
#include "playerbarwidget.h"
#include "chest.h"
#include "powergrenadethrowing.h"
#include "map.h"

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(PlayerBarWidget * playerBarWidget,
                       MainWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MapWidget();

    void setPowerGrenadeThrowing(PowerGrenadeThrowing * p) { powerGrenadeThrowing = p; }

private:
    const int mainItemSize = 10;

    QGraphicsScene * scene = new QGraphicsScene();
    MapView * mapView = new MapView(scene, this);
    QHBoxLayout * qhboxLayout = new QHBoxLayout(this);
    Player * player = new Player(scene, mapView);
    PowerGrenadeThrowing * powerGrenadeThrowing = nullptr;
    PlayerBarWidget * playerBarWidget = nullptr;
    Map * map = new Map(scene);

    QTimer timerChestCreating;
    QTimer throwingGrenadeTimer;

    double xPos = 0;
    double yPos = 0;

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    void startThrowingGrenade();
    void checkChestsInArea();

private slots:
    void changeSceneRect();
    void createChest();
    void stopThrowingGrenade();
};

#endif // MAPWIDGET_H
