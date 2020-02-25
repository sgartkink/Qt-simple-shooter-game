#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setLayout(qvboxLayout);

    qvboxLayout->addWidget(mapWidget);
    qvboxLayout->addWidget(playerBarWidget);
    playerBarWidget->setMinimumHeight(100);
}

MainWidget::~MainWidget()
{
    delete qvboxLayout;
    delete playerBarWidget;
    delete mapWidget;
}
