#include "ui/powergrenadethrowing.h"
#include "core/gameclock.h"

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

    setGraphicsEffect(effect);
    animationOpacity = new QPropertyAnimation(effect, "opacity");
    animationOpacity->setDuration(1000);
    animationOpacity->setStartValue(1);
    animationOpacity->setEndValue(0);

    connect(animationOpacity, &QPropertyAnimation::finished, this, [this](){ setVisible(false); });
}

void PowerGrenadeThrowing::start()
{
    animationOpacity->stop();
    setVisible(true);
    effect->setOpacity(1);
    animationValue->start();
    GameClock::track(animationValue);
}

void PowerGrenadeThrowing::stop()
{
    animationValue->stop();
    animationOpacity->start();
    GameClock::track(animationOpacity);
}

PowerGrenadeThrowing::~PowerGrenadeThrowing()
{
    delete animationValue;
    delete animationOpacity;
}
