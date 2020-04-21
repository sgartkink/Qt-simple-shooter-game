#include "messageattopofscreen.h"

MessageAtTopOfScreen::MessageAtTopOfScreen(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setVisible(false);

    setLayout(gridLayout);
    gridLayout->addWidget(&label);

    label.setAlignment(Qt::AlignHCenter);

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(2000);
    animation->setStartValue(1);
    animation->setEndValue(0);

    connect(animation, &QPropertyAnimation::finished, [this](){ setVisible(false); });
}

void MessageAtTopOfScreen::showMessage(QString text)
{
    setVisible(true);
    animation->stop();
    label.setText(text);
    animation->start();
}
