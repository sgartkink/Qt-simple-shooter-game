#include "mapwidget.h"
#include "global_consts.h"
#include "mainwidget.h"

MapWidget::MapWidget(PlayerBarWidget *playerBarWidget, MainWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f),
      playerBarWidget(playerBarWidget)
{
    qhboxLayout->addWidget(mapView);
    setStyleSheet("border: 0px");

    connect(player, &Player::ammoChangedReloading, [playerBarWidget, this]()
        { playerBarWidget->updateAmmoReloading(player->gun()->ammoLoaded(), player->gun()->ammoOwned()); }
    );
    connect(player, &Player::ammoChangedNoReloading, [playerBarWidget, this]()
        { playerBarWidget->updateAmmoNoReloading(player->gun()->ammoLoaded(), player->gun()->ammoOwned()); }
    );
    emit (player->ammoChangedNoReloading());
    connect(player, &Player::hpChanged, [playerBarWidget, this](){ playerBarWidget->updatePlayerHP(player->getHP()); });
    emit (player->hpChanged());
    connect(player, &Player::grenadesChanged, [playerBarWidget, this](){ playerBarWidget->updateGrenades(player->getGrenades()); });
    emit (player->grenadesChanged());
    connect(player, &Player::armorChanged, [playerBarWidget, this](){ playerBarWidget->updateArmor(player->getArmor()); });
    emit (player->armorChanged());
    connect(player, &Player::gunChanged, [playerBarWidget, this](){ playerBarWidget->updateCurrentGun(player->gunENUM()); });
    emit (player->gunChanged());

    connect(mapView, SIGNAL(sendCord(QPoint)), player, SLOT(updateLineHeroMouse(QPoint)));
    connect(mapView, SIGNAL(mousePressed()), player, SLOT(startShooting()));
    connect(mapView, SIGNAL(mouseReleased()), player, SLOT(stopShooting()));
    connect(player, SIGNAL(posChanged()), this, SLOT(changeSceneRect()));
    connect(&timerChestCreating, SIGNAL(timeout()), this, SLOT(createChest()));
    timerChestCreating.start(30000);

    connect(&throwingGrenadeTimer, SIGNAL(timeout()), this, SLOT(stopThrowingGrenade()));

    scene->addItem(player);
    player->setPos(200,240);

    Bot * bot = new Bot(scene, map);
    bot->setPos(450,300);
    scene->addItem(bot);

    Chest * chest = new Chest;
    scene->addItem(chest);
    chest->setPos(320, 240);

    QTimer::singleShot(500, [this, bot](){ player->start(); bot->start(); });
}

void MapWidget::changeSceneRect()
{
    if (player->pos().x() >= mapView->width()/2 && player->pos().x() <= 820)
        xPos = player->pos().x() - mapView->width()/2;

    if (player->pos().y() >= mapView->height()/2 && player->pos().y() <= 820)
        yPos = player->pos().y() - mapView->height()/2;

    mapView->setSceneRect(xPos, yPos, mapView->width(), mapView->height());
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        switch (event->key())
        {
        case Qt::Key_W:
            player->toggleDirectionUp();
            break;
        case Qt::Key_S:
            player->toggleDirectionDown();
            break;
        case Qt::Key_A:
            player->toggleDirectionLeft();
            break;
        case Qt::Key_D:
            player->toggleDirectionRight();
            break;
        case Qt::Key_Q:
            if (player->getGrenades() > 0 && !player->isThrowingGrenade() && !player->isCurrentlyShooting())
                startThrowingGrenade();
            break;
        case Qt::Key_E:
            checkChestsInArea();
            break;
        case Qt::Key_R:
            if (!player->isCurrentlyShooting() && !player->gun()->isFull())
            {
                player->gun()->reload();
                emit (player->ammoChangedReloading());
            }
            break;
        case Qt::Key_1:
            player->setCurrentGun(PISTOL);
            emit (static_cast<void>(player->gunChanged()), player->ammoChangedNoReloading());
            break;
        case Qt::Key_2:
            player->setCurrentGun(RIFLE);
            emit (static_cast<void>(player->gunChanged()), player->ammoChangedNoReloading());
            break;
        case Qt::Key_3:
            player->setCurrentGun(SHOTGUN);
            emit (static_cast<void>(player->gunChanged()), player->ammoChangedNoReloading());
            break;
        }
}

void MapWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        switch (event->key())
        {
        case Qt::Key_W:
            player->toggleDirectionUp();
            break;
        case Qt::Key_D:
            player->toggleDirectionRight();
            break;
        case Qt::Key_S:
            player->toggleDirectionDown();
            break;
        case Qt::Key_A:
            player->toggleDirectionLeft();
            break;
        case Qt::Key_Q:
            if (player->getGrenades() > 0 && player->isThrowingGrenade() && !player->isCurrentlyShooting())
                stopThrowingGrenade();
            break;
        }
}

void MapWidget::startThrowingGrenade()
{
    player->toggleThrowingGrenade();
    powerGrenadeThrowing->start();
    throwingGrenadeTimer.start(1000);
}

void MapWidget::stopThrowingGrenade()
{
    powerGrenadeThrowing->stop();
    throwingGrenadeTimer.stop();
    player->throwGrenade(powerGrenadeThrowing->value(), mapView);
    player->toggleThrowingGrenade();
}

void MapWidget::checkChestsInArea()
{
    QPainterPath path;
    path.addRect(player->x()-20, player->y()- 20, 50,50);

    QList<QGraphicsItem *> listOfItemsWithinRange = scene->items(path);
    for (auto it = listOfItemsWithinRange.begin(); it != listOfItemsWithinRange.end(); it++)
        if (Chest * c = dynamic_cast<Chest*>(*it))
            c->open(player, scene, xPos, yPos);
}

void MapWidget::createChest()
{
    QPoint p;
    do {
        p = QPoint(QRandomGenerator::global()->bounded(248), QRandomGenerator::global()->bounded(248));
    } while(map->checkIfPointIsTaken(p));

    map->addChest(p);
}

MapWidget::~MapWidget()
{
    delete map;
    delete player;
    delete powerGrenadeThrowing;
    delete qhboxLayout;
    delete mapView;
}
