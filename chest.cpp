#include "chest.h"

Chest::Chest()
    : ItemsOnScene(Qt::yellow, 0, 0, 20, 10)
{
    height = 5;
}

void Chest::open(Hero *opener, QGraphicsScene *scene, double howFarToMoveLabelX, double howFarToMoveLabelY)
{
    QLabel * label = new QLabel();
    label->setGeometry(howFarToMoveLabelX, 10+howFarToMoveLabelY, 1024, 50);
    label->setAlignment(Qt::AlignHCenter);
    label->setWindowFlag(Qt::FramelessWindowHint);
    label->setAttribute(Qt::WA_NoSystemBackground);
    switch (content)
    {
    case 0:
        opener->addGrenades(1);
        label->setText("You received: grenade(1)");
        break;
    case 1:
        opener->refillCurrentOwnedAmmo();
        label->setText("You received: refill current owned ammo");
        break;
    case 2:
        opener->refillCurrentLoadedAmmo();
        label->setText("You received: refill current loaded ammo");
        break;
    case 3:
        opener->gun()->addAmmo(20);
        label->setText("You received: ammo(20)");
        break;
    case 4:
        opener->addHP(5);
        label->setText("You received: hp(5)");
        break;
    case 5:
        opener->addArmor(5);
        label->setText("You received: armor(5)");
        break;
    }
    scene->addWidget(label);
    scene->removeItem(this);

    QPropertyAnimation * animation = new QPropertyAnimation(label, "windowOpacity");
    animation->setDuration(3000);
    animation->setStartValue(1);
    animation->setKeyValueAt(0.8, 0.8);
    animation->setEndValue(0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(3000, [this, label](){ delete label; delete this; });
}

void Chest::open(Hero *opener)
{
    switch (content)
    {
    case 0:
        opener->addGrenades(1);
        break;
    case 1:
        opener->refillCurrentOwnedAmmo();
        break;
    case 2:
        opener->refillCurrentLoadedAmmo();
        break;
    case 3:
        opener->gun()->addAmmo(20);
        break;
    case 4:
        opener->addHP(5);
        break;
    case 5:
        opener->addArmor(5);
        break;
    }
    delete this;
}
