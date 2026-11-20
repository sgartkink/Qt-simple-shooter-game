#include <QtTest>
#include <QKeyEvent>

#include "tests/helpers.h"
#include "ui/mainwindow.h"
#include "weapons/grenadeellipseitem.h"
#include "core/global_consts.h"

// Buildings at grid (40,28) and (40,32) form a wall at x 400..440, y 280..360.
class TestGame : public QObject
{
    Q_OBJECT

private slots:
    void wallsBlockThePlayer();
    void wallsStillBlockUnderExplosions();
    void playerStaysInsideTheMap();
    void cameraFollowsThePlayerButStaysInsideTheMap();
    void heroesOffTheLeaderboardStillCountKillsAndDeaths();
    void chestPickupIsCentredOnThePlayer();
    void killingYourselfIsNotAKill();
    void onlyReleasingQThrowsTheGrenade();
    void realGameRunsWithoutErrors();

private:
    static qreal walk(Player * player, QPointF start, void (Hero::*direction)(bool), int ms);
    static void key(QWidget * widget, QEvent::Type type, int key);
};

qreal TestGame::walk(Player *player, QPointF start, void (Hero::*direction)(bool), int ms)
{
    player->setPos(start);
    (player->*direction)(true);
    runFor(ms);
    (player->*direction)(false);
    return player->x();
}

void TestGame::key(QWidget *widget, QEvent::Type type, int key)
{
    QKeyEvent event(type, key, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

void TestGame::wallsBlockThePlayer()
{
    World world;
    Player * player = world.player();
    player->start();
    QVERIFY(walk(player, QPointF(300, 290), &Hero::setDirectionRight, 200) > 300);
    QCOMPARE(walk(player, QPointF(388, 290), &Hero::setDirectionRight, 200), 388.0);
}

void TestGame::wallsStillBlockUnderExplosions()
{
    // collision used itemAt(), which only sees the topmost item: the explosion circle hid the wall
    World world;
    Player * player = world.player();
    player->start();
    GrenadeEllipseItem * explosion = new GrenadeEllipseItem();
    explosion->setRect(-40, -40, 80, 80);
    explosion->setPos(400, 300);
    world.scene()->addItem(explosion);
    QCOMPARE(walk(player, QPointF(388, 290), &Hero::setDirectionRight, 200), 388.0);
    QVERIFY(walk(player, QPointF(362, 330), &Hero::setDirectionRight, 100) > 362);   // open ground still fine
    delete explosion;
}

void TestGame::playerStaysInsideTheMap()
{
    // the edge check used to run before the step, letting the player 2 px out
    World world;
    Player * player = world.player();
    player->start();
    struct Edge { QPointF start; void (Hero::*direction)(bool); };
    const Edge edges[] = { {{6, 700}, &Hero::setDirectionLeft}, {{1484, 700}, &Hero::setDirectionRight},
                           {{700, 6}, &Hero::setDirectionUp}, {{700, 1484}, &Hero::setDirectionDown} };
    for (const Edge & edge : edges)
    {
        walk(player, edge.start, edge.direction, 300);
        QVERIFY(player->x() >= 0 && player->x() <= MAP_SIZE_X_FULL - 10);
        QVERIFY(player->y() >= 0 && player->y() <= MAP_SIZE_Y_FULL - 10);
        QVERIFY(player->x() == 0 || player->x() == MAP_SIZE_X_FULL - 10 || player->y() == 0 || player->y() == MAP_SIZE_Y_FULL - 10);
    }
}

void TestGame::cameraFollowsThePlayerButStaysInsideTheMap()
{
    World world;
    Player * player = world.player();
    MapView * view = world.view();
    const QSize screen = view->viewport()->size();
    const QPointF spots[] = { {0, 0}, {1490, 1490}, {0, 1490}, {1490, 0}, {700, 700}, {1400, 100} };
    for (QPointF spot : spots)
    {
        player->setPos(spot);
        emit player->posChanged();
        QCoreApplication::processEvents();
        const QRectF shown = view->mapToScene(view->viewport()->rect()).boundingRect();
        const QString where = QString("player at (%1,%2): shows %3,%4 to %5,%6").arg(spot.x()).arg(spot.y())
                .arg(shown.left()).arg(shown.top()).arg(shown.right()).arg(shown.bottom());
        QVERIFY2(shown.left() >= 0 && shown.top() >= 0, qPrintable(where));
        QVERIFY2(shown.right() <= MAP_SIZE_X_FULL && shown.bottom() <= MAP_SIZE_Y_FULL, qPrintable(where));
        QCOMPARE(view->sceneRect().size(), QSizeF(screen));
        QVERIFY2(player->sceneBoundingRect().intersects(shown), qPrintable(where));
    }
    player->setPos(1490, 1490);                      // in the corner the screen ends exactly at the map's edge
    emit player->posChanged();
    QCOMPARE(view->sceneRect().bottomRight(), QPointF(MAP_SIZE_X_FULL, MAP_SIZE_Y_FULL));
    player->setPos(695, 695);                        // in the open the player is in the middle
    emit player->posChanged();
    QCOMPARE(view->sceneRect().center(), QPointF(700, 700));
}

void TestGame::heroesOffTheLeaderboardStillCountKillsAndDeaths()
{
    // the counters updated the hero's leaderboard row without checking it had one, and crashed
    HeroStats stats(nullptr);
    stats.increaseKills();
    stats.increaseDeath();
    QCOMPARE(stats.getKills(), 1u);
    QCOMPARE(stats.getDeaths(), 1u);

    World world;
    Bot * bot = new Bot(&world.mapWidget);           // on the map, not on the leaderboard
    bot->setPos(700, 700);
    world.scene()->addItem(bot);
    bot->start();
    world.player()->start();
    bot->attackItem(1000, world.player());
    QCOMPARE(bot->getHeroStats()->getDeaths(), 1u);
    world.player()->attackItem(1000, bot);
    QCOMPARE(bot->getHeroStats()->getKills(), 1u);
}

void TestGame::chestPickupIsCentredOnThePlayer()
{
    World world;
    Player * player = world.player();
    player->start();
    player->setPos(100, 100);                        // centre (105,105)
    world.map()->addChest(QPointF(12, 12));          // 21 px from the centre, 28 px from the top-left corner
    key(&world.mapWidget, QEvent::KeyPress, Qt::Key_E);
    QCOMPARE(chestsIn(world.scene()), 0);
    world.map()->addChest(QPointF(5, 6));            // out of reach
    key(&world.mapWidget, QEvent::KeyPress, Qt::Key_E);
    QCOMPARE(chestsIn(world.scene()), 1);
}

void TestGame::killingYourselfIsNotAKill()
{
    World world;
    Player * player = world.player();
    player->start();
    player->attackItem(100, player);                 // your own grenade
    QCOMPARE(player->getHeroStats()->getDeaths(), 1u);
    QCOMPARE(player->getHeroStats()->getKills(), 0u);

    Bot * bot = world.addBot(QPointF(700, 700));
    bot->start();
    bot->attackItem(100, player);
    QCOMPARE(player->getHeroStats()->getKills(), 1u);
}

void TestGame::onlyReleasingQThrowsTheGrenade()
{
    // any key release used to throw a charging grenade
    World world;
    Player * player = world.player();
    player->start();
    const int grenades = player->getGrenades();
    key(&world.mapWidget, QEvent::KeyPress, Qt::Key_Q);
    QVERIFY(player->isThrowingGrenade());
    for (int other : {Qt::Key_E, Qt::Key_R, Qt::Key_1, Qt::Key_Space})
        key(&world.mapWidget, QEvent::KeyRelease, other);
    QVERIFY(player->isThrowingGrenade());
    QCOMPARE(player->getGrenades(), grenades);
    key(&world.mapWidget, QEvent::KeyRelease, Qt::Key_Q);
    QVERIFY(!player->isThrowingGrenade());
    QCOMPARE(player->getGrenades(), grenades - 1);
}

void TestGame::realGameRunsWithoutErrors()
{
    // the real window for 8 s: the player keeps appearing in front of and behind the bot and firing, chests spawn,
    // a grenade goes off. Mainly a smoke test; build with -fsanitize=address to catch memory errors as well
    MainWindow window;
    window.show();
    runFor(100);
    window.startProgram();
    runFor(3300);
    MapWidget * mapWidget = window.findChild<MapWidget *>();
    QVERIFY(mapWidget);
    Player * player = nullptr;
    Bot * bot = nullptr;
    for (QGraphicsItem * item : mapWidget->getScene()->items())
    {
        if (Player * p = dynamic_cast<Player *>(item)) player = p;
        if (Bot * b = dynamic_cast<Bot *>(item)) bot = b;
    }
    QVERIFY(player && bot);

    qreal botWalked = 0;
    QPointF last = bot->pos();
    for (int round = 0; round < 8; round++)
    {
        if (player->isAlive() && bot->isAlive())
        {
            const qreal facing = qDegreesToRadians(bot->lineAngle() + (round % 2 ? 0 : 180));
            player->setPos(bot->pos() + QPointF(80 * qCos(facing), -80 * qSin(facing)));
            emit player->posChanged();
            player->setCurrentGun(round % 3);
            player->startShooting();
        }
        if (round % 3 == 0)
            mapWidget->getMap()->addChest(mapWidget->randNewFreePos());
        if (round == 4 && player->getGrenades() > 0)
            player->throwGrenade(40, window.findChild<MapView *>());
        for (int t = 0; t < 20; t++)
        {
            runFor(50);
            botWalked += QLineF(last, bot->pos()).length();
            last = bot->pos();
        }
        player->stopShooting();
    }
    QVERIFY(botWalked > 100);
}

int runTestGame(int argc, char *argv[])
{
    TestGame test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_game.moc"
