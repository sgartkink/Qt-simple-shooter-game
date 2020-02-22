#ifndef PLAYERBARWIDGET_H
#define PLAYERBARWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLCDNumber>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>

class PlayerBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlayerBarWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

public slots:
    void updateCurrentGun(int gun);
    void updatePlayerHP(int hp);
    void updateGrenades(int grenades);
    void updateArmor(int armor);
    void updateAmmoNoReloading(int ammoLoaded, int ammoOwned);
    void updateAmmoReloading(int ammoLoaded, int ammoOwned);

private:
    QProgressBar * reloadingBar = new QProgressBar();

    QGridLayout gridLayout;
    QLCDNumber qlcdLoadedAmmo;
    QLCDNumber qlcdOwnedAmmo;
    QLCDNumber qlcdPlayerHP;
    QLCDNumber qlcdGrenades;
    QLCDNumber qlcdArmor;
    QLabel textCurrentGun;
    QLabel currentGun;

    void reloadingAnimation();
};

#endif // PLAYERBARWIDGET_H
