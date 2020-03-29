#include "optionswidget.h"

OptionsWidget::OptionsWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qvBoxLayout->addWidget(lUp);
    qvBoxLayout->addWidget(lDown);
    qvBoxLayout->addWidget(lRight);
    qvBoxLayout->addWidget(lLeft);
    qvBoxLayout->addWidget(lReload);
    qvBoxLayout->addWidget(lGrenade);
    qvBoxLayout->addWidget(lChest);
    qvBoxLayout->addWidget(lGuns);

    qvBoxLayout->addWidget(bBack);

    connect(bBack, SIGNAL(clicked()), parent, SLOT(backToMenu()));
}

OptionsWidget::~OptionsWidget()
{
    delete lUp;
    delete lDown;
    delete lRight;
    delete lLeft;
    delete lReload;
    delete lGrenade;
    delete lChest;
    delete lGuns;
    delete bBack;
}
