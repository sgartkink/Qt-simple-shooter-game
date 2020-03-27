#include "playerbarwidget.h"
#include "global_consts.h"
#include <QtDebug>

PlayerBarWidget::PlayerBarWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    setLayout(&gridLayout);

    gridLayout.setMargin(0);
    gridLayout.setSpacing(0);
    gridLayout.setColumnStretch(0, this->width()/3);
    gridLayout.setColumnStretch(1, this->width()/3);
    gridLayout.setColumnStretch(2, this->width()/3);

    gridLayout.addWidget(&textLoadedAmmo, 0, 0);
    gridLayout.addWidget(&qlcdLoadedAmmo, 0, 1);
    gridLayout.addWidget(&textOwnedAmmo, 0, 2);
    gridLayout.addWidget(&qlcdOwnedAmmo, 0, 3);
    gridLayout.addWidget(&textCurrentGun, 0, 4, Qt::AlignHCenter);
    gridLayout.addWidget(&textHP, 1, 0);
    gridLayout.addWidget(&qlcdPlayerHP, 1, 1);
    gridLayout.addWidget(&textGrenades, 1, 2);
    gridLayout.addWidget(&qlcdGrenades, 1, 3);
    gridLayout.addWidget(&currentGun, 1, 4, 2, 1, Qt::AlignCenter);
    gridLayout.addWidget(&textArmor, 2, 0);
    gridLayout.addWidget(&qlcdArmor, 2, 1);
    gridLayout.addWidget(reloadingBar, 2, 2, 1, 3);

    textCurrentGun.setText("Current gun:");
    textLoadedAmmo.setText("Loaded ammo:");
    textOwnedAmmo.setText("Owned ammo:");
    textHP.setText("HP:");
    textGrenades.setText("Grenades:");
    textArmor.setText("Armor:");

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

void PlayerBarWidget::updateAmmoReloading(int ammoLoaded, int ammoOwned)
{
    reloadingAnimation();
    QTimer::singleShot(RELOADING_TIME, [this, ammoLoaded, ammoOwned]()
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
}

PlayerBarWidget::~PlayerBarWidget()
{
    delete reloadingBar;
}
