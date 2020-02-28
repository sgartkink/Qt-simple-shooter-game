#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setLayout(qvboxLayout);

    mapWidget = new MapWidget(playerBarWidget, this);

    powerGrenadeThrowing = new PowerGrenadeThrowing(this);
//    powerGrenadeThrowing->show();
    powerGrenadeThrowing->move(20,10);
    powerGrenadeThrowing->setMinimumWidth(300);

    playerBarWidget->setMinimumHeight(100);

    qvboxLayout->addWidget(mapWidget);
    qvboxLayout->addWidget(playerBarWidget);

    mapWidget->setPowerGrenadeThrowing(powerGrenadeThrowing);
}

MainWidget::~MainWidget()
{
    delete qvboxLayout;
    delete playerBarWidget;
    delete mapWidget;
}
