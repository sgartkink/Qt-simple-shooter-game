#include "mapwidget.h"
#include "global_consts.h"
#include "mainwidget.h"
#include <QtDebug>

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
    connect(player, &Player::grenadesChanged, [playerBarWidget, this](){
        playerBarWidget->updateGrenades(player->getGrenades());
    });
    emit (player->grenadesChanged());
    connect(player, &Player::armorChanged, [playerBarWidget, this](){ playerBarWidget->updateArmor(player->getArmor()); });
    emit (player->armorChanged());
    connect(player, &Player::gunChanged, [playerBarWidget, this](){ playerBarWidget->updateCurrentGun(player->gunENUM()); });
    emit (player->gunChanged());

    connect(mapView, SIGNAL(sendCord(QPoint)), player, SLOT(updateLineHeroMouse(QPoint)));
    connect(player, SIGNAL(posChanged()), this, SLOT(changeSceneRect()));
    connect(&throwingGrenadeTimer, SIGNAL(timeout()), this, SLOT(stopThrowingGrenade()));
    connect(&timerChestCreating, SIGNAL(timeout()), this, SLOT(createChest()));

    scene->addItem(player);
    player->setPos(200,240);

    Chest * chest = new Chest;
    scene->addItem(chest);
    chest->setPos(320, 240);
}

void MapWidget::start()
{
    mapView_halfWidth = mapView->width()/2;
    mapView_halfHeight = mapView->height()/2;

    Bot * bot = new Bot(this);
    bot->setPos(450,300);
    scene->addItem(bot);

    leaderboard->addNewHero(player->getHeroStats());
    leaderboard->addNewHero(bot->getHeroStats());

    CountdownToStart * countdownToStart = new CountdownToStart(scene, mapView->width(), mapView->height());

    connect(countdownToStart, &QObject::destroyed, [this, bot](){
        connect(mapView, SIGNAL(mousePressed()), player, SLOT(startShooting()));
        connect(mapView, SIGNAL(mouseReleased()), player, SLOT(stopShooting()));
        timerChestCreating.start(30000);
        player->start();
        bot->start();
        gameStarted = true;
        disconnect(this);
    });
}

QPoint MapWidget::randNewFreePos()
{
    QPoint p;
    do {
        p = QPoint(QRandomGenerator::global()->bounded(MAP_SIZE_X_10), QRandomGenerator::global()->bounded(MAP_SIZE_Y_10));
    } while(map->checkIfPointIsTaken(p) || map->checkIfPointIsTaken(QPoint(p.x()+1, p.y())));

    return p;
}

void MapWidget::changeSceneRect()
{
    if (player->pos().x() >= mapView_halfWidth && player->pos().x() <= MAP_SIZE_X_FULL - mapView_halfWidth)
        xPos = player->pos().x() - mapView_halfWidth;
    else if (player->pos().x() < mapView_halfWidth)
        xPos = 0;
    else
        xPos = MAP_SIZE_X_FULL - mapView->width();

    if (player->pos().y() >= mapView_halfHeight && player->pos().y() <= MAP_SIZE_Y_FULL - mapView_halfHeight)
        yPos = player->pos().y() - mapView_halfHeight;
    else if (player->pos().y() < mapView_halfHeight)
        yPos = 0;
    else
        yPos = MAP_SIZE_Y_FULL - mapView->height();

    mapView->setSceneRect(xPos, yPos, mapView->width(), mapView->height());
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        switch (event->key())
        {
        case Qt::Key_W:
            player->setDirectionUp(true);
            break;
        case Qt::Key_S:
            player->setDirectionDown(true);
            break;
        case Qt::Key_A:
            player->setDirectionLeft(true);
            break;
        case Qt::Key_D:
            player->setDirectionRight(true);
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
        case Qt::Key_X:
            leaderboard->setVisible(1);
            break;
        default:
            if (gameStarted)
                switch (event->key())
                {
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
                }
        }
}

void MapWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat())
        switch (event->key())
        {
        case Qt::Key_W:
            player->setDirectionUp(false);
            break;
        case Qt::Key_D:
            player->setDirectionRight(false);
            break;
        case Qt::Key_S:
            player->setDirectionDown(false);
            break;
        case Qt::Key_A:
            player->setDirectionLeft(false);
            break;
        case Qt::Key_X:
            leaderboard->setVisible(0);
            break;
        default:
            if (gameStarted)
                if (player->getGrenades() > 0 && player->isThrowingGrenade() && !player->isCurrentlyShooting())
                    stopThrowingGrenade();
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
    path.addEllipse(player->pos(), 25, 25);

    QList<QGraphicsItem *> listOfItemsWithinRange = scene->items(path);
    for (auto it = listOfItemsWithinRange.begin(); it != listOfItemsWithinRange.end(); it++)
        if (Chest * c = dynamic_cast<Chest*>(*it))
            c->open(player, scene, xPos, yPos);
}

void MapWidget::createChest()
{
    map->addChest(randNewFreePos());
}

MapWidget::~MapWidget()
{
    delete map;
    delete player;
    delete powerGrenadeThrowing;
    delete qhboxLayout;
    delete mapView;
}
