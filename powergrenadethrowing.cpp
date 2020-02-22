#include "powergrenadethrowing.h"

PowerGrenadeThrowing::PowerGrenadeThrowing(QWidget *parent)
    : QProgressBar(parent)
{
    setMinimum(0);
    setMaximum(100);
    setVisible(false);

    animationValue = new QPropertyAnimation(this, "value");
    animationValue->setDuration(1000);
    animationValue->setStartValue(0);
    animationValue->setEndValue(100);

    animationOpacity = new QPropertyAnimation(this, "windowOpacity");
    animationOpacity->setDuration(1000);
    animationOpacity->setStartValue(1);
    animationOpacity->setEndValue(0);

    connect(animationOpacity, &QPropertyAnimation::finished, [this](){ setVisible(false); });
}

void PowerGrenadeThrowing::start()
{
    animationOpacity->stop();
    setWindowOpacity(1);
    animationValue->start();
    setVisible(true);
}

void PowerGrenadeThrowing::stop()
{
    animationValue->stop();
    animationOpacity->start();
}
