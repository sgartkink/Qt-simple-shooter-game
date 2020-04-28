#include "mainwidget.h"
#include "global_consts.h"
#include <QtDebug>
MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    playerBarWidget->setMinimumHeight(100);

    countdown = new Countdown(this);
    countdown->move(0, (WINDOW_HEIGHT-playerBarWidget->height())/2-50);
    countdown->setMinimumHeight(50);
    countdown->setMinimumWidth(WINDOW_WIDTH);

    messageAtTopOfScreen = new MessageAtTopOfScreen(this);
    messageAtTopOfScreen->move(0, 20);
    messageAtTopOfScreen->setMinimumWidth(WINDOW_WIDTH);

    powerGrenadeThrowing = new PowerGrenadeThrowing(this);
    powerGrenadeThrowing->move(20,10);
    powerGrenadeThrowing->setMinimumWidth(300);

    leaderboard = new Leaderboard(this);
    leaderboard->move(0,0);
    leaderboard->setMinimumWidth(WINDOW_WIDTH);
    leaderboard->setMinimumHeight(WINDOW_HEIGHT - playerBarWidget->height());

    mapWidget->setPowerGrenadeThrowing(powerGrenadeThrowing);
    mapWidget->setLeaderboard(leaderboard);
    mapWidget->setMessageAtTopOfScreen(messageAtTopOfScreen);
    mapWidget->setCountdown(countdown);

    setLayout(qvboxLayout);
    qvboxLayout->addWidget(mapWidget);
    qvboxLayout->addWidget(playerBarWidget);
}

MainWidget::~MainWidget()
{
    delete qvboxLayout;
    delete playerBarWidget;
    delete mapWidget;
}
