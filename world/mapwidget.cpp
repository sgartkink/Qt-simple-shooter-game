#include "world/mapwidget.h"
#include "core/global_consts.h"
#include "ui/mainwidget.h"
#include <QtDebug>

MapWidget::MapWidget(PlayerBarWidget *playerBarWidget, MainWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f),
      playerBarWidget(playerBarWidget)
{
    qhboxLayout->addWidget(mapView);
    setStyleSheet("border: 0px");

    connect(player, &Player::ammoChangedReloading, playerBarWidget, [playerBarWidget, this]()
        { playerBarWidget->updateAmmoReloading(player->gun()->ammoLoaded(), player->gun()->ammoOwned()); }
    );
    connect(player, &Player::ammoChangedNoReloading, playerBarWidget, [playerBarWidget, this]()
        { playerBarWidget->updateAmmoNoReloading(player->gun()->ammoLoaded(), player->gun()->ammoOwned()); }
    );
    emit (player->ammoChangedNoReloading());
    connect(player, &Player::hpChanged, playerBarWidget, [playerBarWidget, this](){ playerBarWidget->updatePlayerHP(player->getHP()); });
    emit (player->hpChanged());
    connect(player, &Player::grenadesChanged, playerBarWidget, [playerBarWidget, this](){
        playerBarWidget->updateGrenades(player->getGrenades());
    });
    emit (player->grenadesChanged());
    connect(player, &Player::armorChanged, playerBarWidget, [playerBarWidget, this](){ playerBarWidget->updateArmor(player->getArmor()); });
    emit (player->armorChanged());
    connect(player, &Player::gunChanged, playerBarWidget, [playerBarWidget, this](){ playerBarWidget->updateCurrentGun(player->gunENUM()); });
    emit (player->gunChanged());

    connect(mapView, &MapView::sendCord, player, &Player::updateLineHeroMouse);
    connect(player, &Player::posChanged, this, &MapWidget::changeSceneRect);
    connect(player, &Player::posChanged, this, &MapWidget::updateFieldOfView);
    connect(player, &Player::viewChanged, this, &MapWidget::updateFieldOfView);
    connect(player, &Player::died, mapView, &MapView::clearShot);
    fieldOfViewTimer.setSingleShot(true);
    fieldOfViewTimer.setInterval(FIELD_OF_VIEW_FRAME);
    connect(&fieldOfViewTimer, &QTimer::timeout, this, [this]()
    {
        if (fieldOfViewPending)
        {
            applyFieldOfView();
            fieldOfViewTimer.start();
        }
    });
    connect(&throwingGrenadeTimer, &GameTimer::timeout, this, &MapWidget::stopThrowingGrenade);
    roundTimer.setSingleShot(true);
    connect(&roundTimer, &GameTimer::timeout, this, &MapWidget::roundTimeUp);
    connect(&roundClock, &GameTimer::timeout, this, &MapWidget::updateRoundClock);
    updateRoundClock();
    connect(&timerChestCreating, &GameTimer::timeout, this, &MapWidget::createChest);

    scene->addItem(player);
    player->setPos(200,240);
    applyFieldOfView();
}

void MapWidget::start()
{
    bot = new Bot(this);
    bot->setPos(450,300);
    scene->addItem(bot);

    leaderboard->addNewHero(player->getHeroStats());
    leaderboard->addNewHero(bot->getHeroStats());

    countdown->start("Game starts in:", 3);

    GameClock::singleShot(3000, this, [this](){
        roundTimer.start(roundLengthMs);
        roundEndsAt = GameClock::now() + roundLengthMs;
        roundClock.start(250);
        connect(mapView, &MapView::mousePressed, player, &Player::startShooting);
        connect(mapView, &MapView::mouseReleased, player, &Player::stopShooting);
        timerChestCreating.start(30000);
        player->start();
        bot->start();
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

void MapWidget::showCountdown(const QString &text, const int &s)
{
    if (countdown)
        countdown->start(text, s);
}

int MapWidget::roundMsLeft() const
{
    if (roundEndsAt < 0)
        return roundLengthMs;
    return int(qMax<qint64>(0, roundEndsAt - GameClock::now()));
}

QVector<HeroStats *> MapWidget::heroStats()
{
    QVector<HeroStats *> stats {player->getHeroStats()};
    if (bot)
        stats.append(bot->getHeroStats());
    return stats;
}

void MapWidget::releaseControls()
{
    player->setDirectionUp(false);
    player->setDirectionDown(false);
    player->setDirectionLeft(false);
    player->setDirectionRight(false);
    player->stopShooting();
    if (leaderboard)
        leaderboard->setVisible(false);
}

void MapWidget::focusGame()
{
    mapView->setFocus();
}

void MapWidget::roundTimeUp()
{
    // the game clock decides: a timer may fire a few ms early, and the round would end with time still on it
    const int left = roundMsLeft();
    if (left > 0)
        roundTimer.start(left);
    else
        endRound();
}

void MapWidget::endRound()
{
    roundIsOver = true;
    roundEndsAt = GameClock::now();     // the timer may fire a few ms early; time is up all the same
    roundClock.stop();
    updateRoundClock();
    releaseControls();
    GameClock::setPaused(true);     // everything stops where it is
    emit roundOver();
}

void MapWidget::updateRoundClock()
{
    if (playerBarWidget)
        playerBarWidget->updateTimeLeft((roundMsLeft() + 999) / 1000);
}

void MapWidget::heroAttacked(Hero *attacker, QPointF origin)
{
    if (attacker != player && player->isAlive())
        mapView->showShot(player, origin);
}

void MapWidget::changeSceneRect()
{
    // centred on the player, but never past the map's edge; sized to the viewport (what is actually shown),
    // measured each time, so it stays right if the window size changes
    const QSize screen = mapView->viewport()->size();
    const QPointF centre = player->pos() + QPointF(mainItemSize / 2.0, mainItemSize / 2.0);
    const qreal x = qBound<qreal>(0, centre.x() - screen.width() / 2.0, MAP_SIZE_X_FULL - screen.width());
    const qreal y = qBound<qreal>(0, centre.y() - screen.height() / 2.0, MAP_SIZE_Y_FULL - screen.height());
    mapView->setSceneRect(x, y, screen.width(), screen.height());
}

void MapWidget::updateFieldOfView()
{
    // at most one refresh per frame: a fast mouse would otherwise redraw the darkness hundreds of times a second.
    // The first change is drawn at once, later ones within the frame are collected and the latest is drawn when it ends
    if (fieldOfViewTimer.isActive())
        fieldOfViewPending = true;
    else
    {
        applyFieldOfView();
        fieldOfViewTimer.start();
    }
}

void MapWidget::applyFieldOfView()
{
    fieldOfViewPending = false;
    mapView->setVisibleArea(fieldOfView.visibleArea(player->mapToScene(player->rect().center()), player->lineAngle()));
}

void MapWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (!event->isAutoRepeat() && !roundIsOver)
            emit pauseRequested();
        return;
    }
    if (GameClock::isPaused())      // paused or the round is over: the controls were let go and stay so
        return;

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
            if (player->isAlive())
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
    if (GameClock::isPaused())
        return;

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
        case Qt::Key_Q:
            if (player->isAlive())
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
    path.addEllipse(player->mapToScene(player->rect().center()), 25, 25);

    QList<QGraphicsItem *> listOfItemsWithinRange = scene->items(path);
    for (auto it = listOfItemsWithinRange.begin(); it != listOfItemsWithinRange.end(); it++)
        if (Chest * c = dynamic_cast<Chest*>(*it))
        {
            c->open(player, messageAtTopOfScreen);
            map->removeChest(c);
        }
}

void MapWidget::createChest()
{
    map->addChest(randNewFreePos());
}

MapWidget::~MapWidget()
{
    timerChestCreating.stop();
    throwingGrenadeTimer.stop();
    fieldOfViewTimer.stop();

    // bullets, grenades and bots run their own timers and point at heroes, so they go first, then the player and the
    // map, and the scene last: it deletes whatever is left on it (buildings, chests, the map's edge lines)
    for (QGraphicsItem * item : scene->items())
        if (dynamic_cast<Bullet *>(item) || dynamic_cast<Grenade *>(item) || dynamic_cast<Bot *>(item))
            delete item;
    delete player;
    delete map;
    delete powerGrenadeThrowing;
    delete qhboxLayout;
    delete mapView;
    delete scene;
}
