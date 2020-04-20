#include "messageattopofscreen.h"

MessageAtTopOfScreen::MessageAtTopOfScreen(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setVisible(false);

    setLayout(gridLayout);
    gridLayout->addWidget(&label);

    label.setAlignment(Qt::AlignHCenter);

    animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(2000);
    animation->setStartValue(1);
    animation->setEndValue(0);
}

void MessageAtTopOfScreen::showMessage(QString text)
{
    setVisible(true);
    animation->stop();
    label.setText(text);
    animation->start();

    QTimer::singleShot(2000, [this](){ this->setVisible(false); });
}
