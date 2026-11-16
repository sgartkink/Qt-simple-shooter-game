#include <QtTest>

#include "tests/helpers.h"
#include "world/shotmarker.h"

// (700,700) is open ground; the 1024x580 view puts the player's centre near the middle of the screen
class TestShotMarker : public QObject
{
    Q_OBJECT

private slots:
    void fainterWithDistanceAndNoneBeyondThreeQuartersOfTheScreen();
    void holdsThenFadesOut();
    void anEnemyShotIsMarkedEvenWhenItMisses();
    void beingHitAloneMarksNothing();
    void yourOwnShotsAreNotMarked();
    void farShotsAreNotMarked();
    void everyShotBlinksAgainAndPointsAtTheNewShooter();
    void dyingClearsItAndNothingIsMarkedWhileDead();
    void drawnCloseToTheHeroTowardsTheShooter();

private:
    static QPointF centre(QGraphicsItem * item) { return item->sceneBoundingRect().center(); }
    static Player * readyPlayer(World & world);
    static void enemyFiredFrom(World & world, Bot * bot, QPointF origin) { world.mapWidget.heroFired(bot, origin); }
};

Player * TestShotMarker::readyPlayer(World &world)
{
    Player * player = world.player();
    player->setPos(700, 700);
    MapView * view = world.view();               // the camera centred on the player, as in the game
    view->setSceneRect(705 - view->viewport()->width() / 2, 705 - view->viewport()->height() / 2,
                       view->viewport()->width(), view->viewport()->height());
    player->start();
    QCoreApplication::processEvents();
    return player;
}

void TestShotMarker::fainterWithDistanceAndNoneBeyondThreeQuartersOfTheScreen()
{
    const QRectF screen(0, 0, 1000, 600);
    const QPointF hero(500, 300);                // 500 px to the side edges, 300 px to the top and bottom
    QVERIFY(ShotMarker::strengthFor(hero, hero + QPointF(5, 0), screen) > 0.98);
    QVERIFY(ShotMarker::strengthFor(hero, hero + QPointF(370, 0), screen) >= ShotMarker::MIN_STRENGTH);
    QCOMPARE(ShotMarker::strengthFor(hero, hero + QPointF(380, 0), screen), 0.0);     // past 75% of 500
    QVERIFY(ShotMarker::strengthFor(hero, hero + QPointF(0, -220), screen) > 0);
    QCOMPARE(ShotMarker::strengthFor(hero, hero + QPointF(0, -230), screen), 0.0);    // past 75% of 300
    qreal last = 2;
    for (int d = 10; d <= 370; d += 20)
    {
        const qreal s = ShotMarker::strengthFor(hero, hero + QPointF(-d, 0), screen);
        QVERIFY2(s < last, qPrintable(QString("not fainter at %1 px").arg(d)));
        last = s;
    }
}

void TestShotMarker::holdsThenFadesOut()
{
    ShotMarker marker;
    QVERIFY(!marker.isActiveAt(0));
    QVERIFY(marker.mark(QPointF(500, 300), QPointF(600, 300), QRectF(0, 0, 1000, 600)));
    QCOMPARE(marker.opacityAt(0), 1.0);            // fully red, whatever the distance
    QCOMPARE(marker.opacityAt(500), 1.0);
    QVERIFY(qAbs(marker.opacityAt(1250) - 0.5) < 1e-9);
    QVERIFY(marker.opacityAt(1990) > 0);
    QCOMPARE(marker.opacityAt(2000), 0.0);
    marker.clear();
    QVERIFY(!marker.isActiveAt(100));
}

void TestShotMarker::anEnemyShotIsMarkedEvenWhenItMisses()
{
    World world;
    Player * player = readyPlayer(world);
    Bot * bot = world.addBot(QPointF(800, 700));     // 100 px to the right, facing right: away from the player
    bot->start();
    bot->getShooting()->start();
    bot->getShooting()->stop();
    QVERIFY(world.view()->isShotMarkerShown());
    QVERIFY(QLineF(world.view()->getShotMarker().getOrigin(), centre(bot)).length() < 0.01);
    runFor(300);
    QCOMPARE(player->getArmor(), 10);                // missed
    runFor(1800);
    QVERIFY(!world.view()->isShotMarkerShown());     // gone after 0.5 s + 1.5 s
}

void TestShotMarker::beingHitAloneMarksNothing()
{
    World world;
    Player * player = readyPlayer(world);
    Bot * bot = world.addBot(QPointF(820, 700));
    Bullet * bullet = new Bullet(bot, 1, -5, 0);     // a bullet flying at the player, with no shot fired
    bullet->setPos(centre(bot));
    world.scene()->addItem(bullet);
    runFor(400);
    QCOMPARE(player->getArmor(), 9);                 // hit
    QVERIFY(!world.view()->isShotMarkerShown());
}

void TestShotMarker::yourOwnShotsAreNotMarked()
{
    World world;
    Player * player = readyPlayer(world);
    player->startShooting();
    runFor(100);
    player->stopShooting();
    QVERIFY(!world.view()->isShotMarkerShown());
}

void TestShotMarker::farShotsAreNotMarked()
{
    World world;
    Player * player = readyPlayer(world);
    Bot * bot = world.addBot(QPointF(100, 100));
    const QPointF me = centre(player);
    enemyFiredFrom(world, bot, me + QPointF(0, -300));    // the screen reaches ~280 px up: well past 75%
    QVERIFY(!world.view()->isShotMarkerShown());
    enemyFiredFrom(world, bot, me + QPointF(0, -150));
    QVERIFY(world.view()->isShotMarkerShown());
}

void TestShotMarker::everyShotBlinksAgainAndPointsAtTheNewShooter()
{
    World world;
    Player * player = readyPlayer(world);
    Bot * bot = world.addBot(QPointF(100, 100));
    const QPointF me = centre(player);
    enemyFiredFrom(world, bot, me + QPointF(100, 0));
    runFor(1500);                                    // half faded
    enemyFiredFrom(world, bot, me + QPointF(0, 100));
    QCOMPARE(world.view()->getShotMarker().getOrigin(), me + QPointF(0, 100));
    runFor(1000);                                    // 2.5 s after the first shot, 1 s after the second
    QVERIFY(world.view()->isShotMarkerShown());
}

void TestShotMarker::dyingClearsItAndNothingIsMarkedWhileDead()
{
    World world;
    Player * player = readyPlayer(world);
    Bot * bot = world.addBot(QPointF(100, 100));
    enemyFiredFrom(world, bot, centre(player) + QPointF(100, 0));
    QVERIFY(world.view()->isShotMarkerShown());
    player->attackItem(1000, bot);
    QVERIFY(!player->isAlive());
    QVERIFY(!world.view()->isShotMarkerShown());
    enemyFiredFrom(world, bot, centre(player) + QPointF(100, 0));
    QVERIFY(!world.view()->isShotMarkerShown());
}

void TestShotMarker::drawnCloseToTheHeroTowardsTheShooter()
{
    World world;
    Player * player = readyPlayer(world);
    MapView * view = world.view();
    const QPointF me = centre(player);
    auto pixelAt = [&](qreal degrees) {
        const QPointF p = me + ShotMarker::RADIUS * QPointF(qCos(qDegreesToRadians(degrees)), -qSin(qDegreesToRadians(degrees)));
        return QColor(view->viewport()->grab().toImage().pixel(view->mapFromScene(p)));
    };
    auto red = [](QColor c) { return c.red() > 180 && c.green() < 60 && c.blue() < 60; };   // properly red, not pink

    Bot * bot = world.addBot(QPointF(100, 100));
    for (qreal shooter : {0.0, 135.0, 270.0})
    {
        enemyFiredFrom(world, bot, me + 150 * QPointF(qCos(qDegreesToRadians(shooter)), -qSin(qDegreesToRadians(shooter))));
        runFor(ShotMarker::BLINK_MS + 50);
        QVERIFY2(red(pixelAt(shooter)), qPrintable(QString("no arc towards %1 deg").arg(shooter)));
        QVERIFY2(red(pixelAt(shooter + 30)), qPrintable(QString("arc too short at %1 deg").arg(shooter)));
        QVERIFY2(!red(pixelAt(shooter + 180)), qPrintable(QString("arc on the far side at %1 deg").arg(shooter)));
        QVERIFY2(!red(pixelAt(shooter + 90)), qPrintable(QString("arc too long at %1 deg").arg(shooter)));
    }
    runFor(2100);
    QVERIFY(!red(pixelAt(270)));                 // faded away and erased
}

int runTestShotMarker(int argc, char *argv[])
{
    TestShotMarker test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_shotmarker.moc"
