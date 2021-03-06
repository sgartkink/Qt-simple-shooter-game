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
#include "countdown.h"
#include "leaderboard.h"
#include "messageattopofscreen.h"

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(PlayerBarWidget * playerBarWidget,
                       MainWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MapWidget();

    void start();
    void setPowerGrenadeThrowing(PowerGrenadeThrowing * p) { powerGrenadeThrowing = p; }
    void setLeaderboard(Leaderboard * l) { leaderboard = l; }
    void setMessageAtTopOfScreen(MessageAtTopOfScreen * m) { messageAtTopOfScreen = m; }
    void setCountdown(Countdown * c) { countdown = c; }

    QGraphicsScene * getScene() { return scene; }
    Map * getMap() { return map; }

    QPoint randNewFreePos();
    void showCountdown(const QString &text, const int &s);

public slots:
    void stopThrowingGrenade();

private:
    const int mainItemSize = 10;

    QGraphicsScene * scene = new QGraphicsScene();
    MapView * mapView = new MapView(scene, this);
    QHBoxLayout * qhboxLayout = new QHBoxLayout(this);
    Map * map = new Map(scene);
    Player * player = new Player(mapView, this);
    PowerGrenadeThrowing * powerGrenadeThrowing = nullptr;
    PlayerBarWidget * playerBarWidget = nullptr;
    Leaderboard * leaderboard = nullptr;
    MessageAtTopOfScreen * messageAtTopOfScreen = nullptr;
    Countdown * countdown = nullptr;

    QTimer timerChestCreating;
    QTimer throwingGrenadeTimer;

    double xPos = 0;
    double yPos = 0;

    int mapView_halfWidth;
    int mapView_halfHeight;

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    void startThrowingGrenade();
    void checkChestsInArea();

private slots:
    void changeSceneRect();
    void createChest();
};

#endif // MAPWIDGET_H
