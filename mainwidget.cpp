#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    playerBarWidget->setMinimumHeight(100);

    countdown = new Countdown(this);
    countdown->move(0, 256);
    countdown->setMinimumHeight(50);
    countdown->setMinimumWidth(1024);

    messageAtTopOfScreen = new MessageAtTopOfScreen(this);
    messageAtTopOfScreen->move(0, 20);
    messageAtTopOfScreen->setMinimumWidth(1024);

    powerGrenadeThrowing = new PowerGrenadeThrowing(this);
    powerGrenadeThrowing->move(20,10);
    powerGrenadeThrowing->setMinimumWidth(300);

    leaderboard = new Leaderboard(this);
    leaderboard->move(50,0);

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
