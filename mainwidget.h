#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "mapwidget.h"
#include "playerbarwidget.h"
#include "powergrenadethrowing.h"
#include "leaderboard.h"
#include "messageattopofscreen.h"
#include "countdown.h"

class MainWidget : public QWidget
{
public:
    explicit MainWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MainWidget();

    MapWidget * getMapWidget() { return mapWidget; }
    PowerGrenadeThrowing * getPowerGrenadeThrowing() { return powerGrenadeThrowing; }

private:
    QVBoxLayout * qvboxLayout = new QVBoxLayout(this);
    PlayerBarWidget * playerBarWidget = new PlayerBarWidget(this);
    MapWidget * mapWidget = new MapWidget(playerBarWidget, this);
    MessageAtTopOfScreen * messageAtTopOfScreen = nullptr;
    Leaderboard * leaderboard = nullptr;
    PowerGrenadeThrowing * powerGrenadeThrowing = nullptr;
    Countdown * countdown = nullptr;
};

#endif // MAINWIDGET_H
