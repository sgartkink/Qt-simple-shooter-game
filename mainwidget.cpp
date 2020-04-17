#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    playerBarWidget->setMinimumHeight(100);

    powerGrenadeThrowing = new PowerGrenadeThrowing(this);
    powerGrenadeThrowing->move(20,10);
    powerGrenadeThrowing->setMinimumWidth(300);

    leaderboard = new Leaderboard(this);
    leaderboard->move(50,0);

    mapWidget->setPowerGrenadeThrowing(powerGrenadeThrowing);
    mapWidget->setLeaderboard(leaderboard);

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
