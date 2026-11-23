#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTimer>
#include "core/gameclock.h"
#include "core/settings.h"
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
    void setRoundLength(int ms) { roundLengthMs = ms; updateRoundClock(); }
    int roundMsLeft() const;
    bool isRoundOver() const { return roundIsOver; }
    Player * getPlayer() { return player; }
    Bot * getBot() { return bot; }
    QVector<HeroStats *> heroStats();
    void releaseControls();     // as if every key and the mouse button were let go
    void focusGame();
    void setPowerGrenadeThrowing(PowerGrenadeThrowing * p) { powerGrenadeThrowing = p; }
    void setLeaderboard(Leaderboard * l) { leaderboard = l; }
    void setMessageAtTopOfScreen(MessageAtTopOfScreen * m) { messageAtTopOfScreen = m; }
    void setCountdown(Countdown * c) { countdown = c; }

    QGraphicsScene * getScene() { return scene; }
    Map * getMap() { return map; }
    MapView * getMapView() { return mapView; }
    const FieldOfView & getFieldOfView() const { return fieldOfView; }
    QSize viewSize() const { return mapView->viewport()->size(); }

    QPoint randNewFreePos();
    void heroAttacked(Hero * attacker, QPointF origin);     // marks an enemy's shot or grenade explosion around the player
    void showCountdown(const QString &text, const int &s);

public slots:
    void stopThrowingGrenade();

signals:
    void pauseRequested();      // Esc
    void roundOver();           // time's up; the game clock is paused

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

    Bot * bot = nullptr;
    GameTimer roundTimer;
    GameTimer roundClock;       // refreshes the time left on the bar
    int roundLengthMs = Settings::DEFAULT_ROUND_MINUTES * 60000;
    qint64 roundEndsAt = -1;    // game time; -1 until the round starts
    bool roundIsOver = false;
    GameTimer timerChestCreating;
    GameTimer throwingGrenadeTimer;
    QTimer fieldOfViewTimer;
    bool fieldOfViewPending = false;
    const int FIELD_OF_VIEW_FRAME = 16;

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    void startThrowingGrenade();
    void endRound();
    void updateRoundClock();
    void checkChestsInArea();

private slots:
    void changeSceneRect();
    void updateFieldOfView();
    void applyFieldOfView();
    void createChest();
};

#endif // MAPWIDGET_H
