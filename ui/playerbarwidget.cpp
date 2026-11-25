#include "ui/playerbarwidget.h"
#include "core/global_consts.h"
#include "core/gameclock.h"
#include <QtDebug>

PlayerBarWidget::PlayerBarWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setLayout(&gridLayout);

    gridLayout.addWidget(&textLoadedAmmo, 0, 0, Qt::AlignRight);
    gridLayout.addWidget(&qlcdLoadedAmmo, 0, 1);
    gridLayout.addWidget(&textOwnedAmmo, 0, 2, Qt::AlignRight);
    gridLayout.addWidget(&qlcdOwnedAmmo, 0, 3);
    gridLayout.addWidget(&textCurrentGun, 0, 4, Qt::AlignCenter);
    gridLayout.addWidget(&textHP, 1, 0, Qt::AlignRight);
    gridLayout.addWidget(&qlcdPlayerHP, 1, 1);
    gridLayout.addWidget(&textGrenades, 1, 2, Qt::AlignRight);
    gridLayout.addWidget(&qlcdGrenades, 1, 3);
    gridLayout.addWidget(&currentGun, 1, 4, 2, 1, Qt::AlignTop | Qt::AlignHCenter);
    gridLayout.addWidget(&textArmor, 2, 0, Qt::AlignRight);
    gridLayout.addWidget(&qlcdArmor, 2, 1);
    gridLayout.addWidget(&textReloardingBar, 2, 2, Qt::AlignRight);
    gridLayout.addWidget(reloadingBar, 2, 3, 1, 2);
    gridLayout.addWidget(&textTimeLeft, 0, 5, Qt::AlignCenter);
    gridLayout.addWidget(&timeLeft, 1, 5, Qt::AlignTop | Qt::AlignHCenter);

    textCurrentGun.setText("Current gun:");
    textLoadedAmmo.setText("Loaded ammo:");
    textOwnedAmmo.setText("Owned ammo:");
    textHP.setText("HP:");
    textGrenades.setText("Grenades:");
    textArmor.setText("Armor:");
    textReloardingBar.setText("Reloading bar:");
    textTimeLeft.setText("Time left:");
    QFont font = timeLeft.font();
    font.setPointSize(font.pointSize() * 2);
    font.setBold(true);
    timeLeft.setFont(font);

    reloadingBar->setMinimum(0);
    reloadingBar->setMaximum(100);
    reloadingBar->setValue(100);
    reloadingBar->setTextVisible(false);
}

void PlayerBarWidget::updateCurrentGun(int gun)
{
    switch (gun)
    {
    case PISTOL:
        currentGun.setText("PISTOL");
        break;
    case RIFLE:
        currentGun.setText("RIFLE");
        break;
    case SHOTGUN:
        currentGun.setText("SHOTGUN");
        break;
    case SNIPER:
        currentGun.setText("SNIPER RIFLE");
        break;
    }
}

void PlayerBarWidget::updatePlayerHP(int hp)
{
    qlcdPlayerHP.display(hp);
}

void PlayerBarWidget::updateGrenades(int grenades)
{
    qlcdGrenades.display(grenades);
}

void PlayerBarWidget::updateArmor(int armor)
{
    qlcdArmor.display(armor);
}

void PlayerBarWidget::updateAmmoNoReloading(int ammoLoaded, int ammoOwned)
{
    qlcdLoadedAmmo.display(ammoLoaded);
    qlcdOwnedAmmo.display(ammoOwned);
}

void PlayerBarWidget::updateTimeLeft(int seconds)
{
    timeLeft.setText(QString("%1:%2").arg(seconds / 60).arg(seconds % 60, 2, 10, QChar('0')));
}

void PlayerBarWidget::updateAmmoReloading(int ammoLoaded, int ammoOwned)
{
    reloadingAnimation();
    GameClock::singleShot(RELOADING_TIME, this, [this, ammoLoaded, ammoOwned]()
        {
            qlcdLoadedAmmo.display(ammoLoaded);
            qlcdOwnedAmmo.display(ammoOwned);
        }
    );
}

void PlayerBarWidget::reloadingAnimation()
{
    QPropertyAnimation * animation = new QPropertyAnimation(reloadingBar, "value");
    animation->setDuration(RELOADING_TIME);
    animation->setStartValue(0);
    animation->setEndValue(100);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    GameClock::track(animation);
}

PlayerBarWidget::~PlayerBarWidget()
{
    delete reloadingBar;
}
