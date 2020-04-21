#include "countdown.h"

Countdown::Countdown(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setVisible(false);

    lText.setAlignment(Qt::AlignHCenter);
    lSecLeft.setAlignment(Qt::AlignHCenter);

    setLayout(qvBoxLayout);
    qvBoxLayout->addWidget(&lText);
    qvBoxLayout->addWidget(&lSecLeft);

    connect(&timer, SIGNAL(timeout()), this, SLOT(countdown()));
}

void Countdown::start(const QString & text, const int & s)
{
    setVisible(true);
    secLeft = s;
    lText.setText(text);
    lSecLeft.setText(QString::number(secLeft));

    timer.start(1000);

    QTimer::singleShot(secLeft*1000, [this](){
        timer.stop();
        this->setVisible(false);
    });
}

void Countdown::countdown()
{
    secLeft--;
    lSecLeft.setText(QString::number(secLeft));
}
