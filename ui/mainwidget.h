#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "world/mapwidget.h"
#include "ui/playerbarwidget.h"
#include "ui/powergrenadethrowing.h"
#include "ui/leaderboard.h"
#include "ui/messageattopofscreen.h"
#include "ui/countdown.h"
#include "ui/pausemenu.h"
#include "ui/roundoverwidget.h"

class MainWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MainWidget();

    MapWidget * getMapWidget() { return mapWidget; }
    PowerGrenadeThrowing * getPowerGrenadeThrowing() { return powerGrenadeThrowing; }
    PlayerBarWidget * getPlayerBarWidget() { return playerBarWidget; }
    PauseMenu * getPauseMenu() { return pauseMenu; }
    RoundOverWidget * getRoundOverWidget() { return roundOverWidget; }

public slots:
    void pause();
    void resume();

signals:
    void backToMenuRequested();
    void playAgainRequested();

private:
    QVBoxLayout * qvboxLayout = new QVBoxLayout(this);
    PlayerBarWidget * playerBarWidget = new PlayerBarWidget(this);
    MapWidget * mapWidget = new MapWidget(playerBarWidget, this);
    MessageAtTopOfScreen * messageAtTopOfScreen = nullptr;
    Leaderboard * leaderboard = nullptr;
    PowerGrenadeThrowing * powerGrenadeThrowing = nullptr;
    Countdown * countdown = nullptr;
    PauseMenu * pauseMenu = nullptr;
    RoundOverWidget * roundOverWidget = nullptr;

    void showRoundOver();
};

#endif // MAINWIDGET_H
