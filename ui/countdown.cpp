#include "ui/countdown.h"

Countdown::Countdown(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setVisible(false);

    lText.setAlignment(Qt::AlignCenter);
    lSecLeft.setAlignment(Qt::AlignCenter);

    setLayout(qvBoxLayout);
    qvBoxLayout->addWidget(&lText);
    qvBoxLayout->addWidget(&lSecLeft);

    connect(&timer, &GameTimer::timeout, this, &Countdown::countdown);
}

void Countdown::start(const QString & text, const int & s)
{
    setVisible(true);
    secLeft = s;
    lText.setText(text);
    lSecLeft.setText(QString::number(secLeft));

    timer.start(1000);

    GameClock::singleShot(secLeft*1000, this, [this](){
        timer.stop();
        this->setVisible(false);
    });
}

void Countdown::countdown()
{
    secLeft--;
    lSecLeft.setText(QString::number(secLeft));
}
