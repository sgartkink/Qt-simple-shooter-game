#include "countdowntostart.h"

CountdownToStart::CountdownToStart(QGraphicsScene *scene, int width, int height)
    : scene(scene)
{
    scene->addWidget(lText);
    scene->addWidget(lSecLeft);
    lSecLeft->setText(QString::number(secLeft));
    lText->setGeometry(width/2 - lText->width()/2, height/2 - lText->height()/2 - lSecLeft->height(),
                       lText->width(), lText->height());
    lSecLeft->setGeometry(width/2 - lSecLeft->width()/2, height/2 - lSecLeft->height()/2,
                          lSecLeft->width(), lSecLeft->height());

    connect(&timer, SIGNAL(timeout()), this, SLOT(countdown()));
    timer.start(1000);

    QTimer::singleShot(secLeft*1000, [this](){ delete this; });
}

void CountdownToStart::countdown()
{
    secLeft--;
    lSecLeft->setText(QString::number(secLeft));
}

CountdownToStart::~CountdownToStart()
{
    delete lText;
    delete lSecLeft;
}
