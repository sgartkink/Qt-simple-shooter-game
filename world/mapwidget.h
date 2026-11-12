#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QRandomGenerator>

class MainWidget;
#include "world/mapview.h"
#include "characters/bot.h"
#include "characters/player.h"
#include "ui/playerbarwidget.h"
#include "world/chest.h"
#include "ui/powergrenadethrowing.h"
#include "world/map.h"
#include "world/fieldofview.h"
#include "ui/countdown.h"
#include "ui/leaderboard.h"
#include "ui/messageattopofscreen.h"

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
    FieldOfView fieldOfView {scene};
    Player * player = new Player(mapView, this);
    PowerGrenadeThrowing * powerGrenadeThrowing = nullptr;
    PlayerBarWidget * playerBarWidget = nullptr;
    Leaderboard * leaderboard = nullptr;
    MessageAtTopOfScreen * messageAtTopOfScreen = nullptr;
    Countdown * countdown = nullptr;

    QTimer timerChestCreating;
    QTimer throwingGrenadeTimer;
    QTimer fieldOfViewTimer;
    bool fieldOfViewPending = false;
    const int FIELD_OF_VIEW_FRAME = 16;

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
    void updateFieldOfView();
    void applyFieldOfView();
    void createChest();
};

#endif // MAPWIDGET_H
