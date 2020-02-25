#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>

#include "mapview.h"
#include "bot.h"
#include "player.h"
#include "playerbarwidget.h"
#include "chest.h"
#include "powergrenadethrowing.h"

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(PlayerBarWidget * playerBarWidget, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags()); 
    ~MapWidget();

private:
    const int mainItemSize = 10;

    QGraphicsScene * scene = new QGraphicsScene();
    MapView * mapView = new MapView(scene, this);
    QHBoxLayout * qhboxLayout = new QHBoxLayout(this);
    Player * player = new Player(scene, mapView);
    PowerGrenadeThrowing * powerGrenadeThrowing = new PowerGrenadeThrowing();
    PlayerBarWidget * playerBarWidget = nullptr;

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
