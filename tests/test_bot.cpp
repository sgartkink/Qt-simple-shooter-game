#include <QtTest>
#include <QSet>

#include "tests/helpers.h"

// Bot is driven one tick at a time (nextMove) with its timers stopped; TestBot is a friend of Bot.
// Buildings at grid (40,28) and (40,32) form a wall at x 400..440, y 280..360; (700,700) is open ground.
class TestBot : public QObject
{
    Q_OBJECT

private slots:
    void noticesLikeThePlayer();
    void noticesChestsOnlyInView();
    void searchesTheLastSeenSpot();
    void searchIsCancelledBySeeingThePlayer();
    void givesUpOnAChestThatIsGone();
    void opensTheChestAtTheEndOfARoute();
    void routesToAChestBehindAWallOnce();
    void deathClearsRouteAndMemory();
    void throwsAGrenadeWhereItLostSightOfThePlayer();
    void throwsAGrenadeWhileReloading();
    void grenadeLimits();
    void grenadeLandsWhereItWasAimed();
    void waitsForItsOwnGrenadeAndStepsAwayFromIt();
    void dodgesAGrenadeItSees();
    void ignoresAGrenadeItDoesNotSee();

private:
    static bool notices(Bot & bot, Player * player, QPointF at, qreal facing, QPointF target);
    static void face(Bot & bot, qreal degrees);
    static QPointF from(QPointF point, qreal degrees, qreal distance);
    static void loseSightOfThePlayer(World & world, Bot & bot, QPointF & spot);
};

void TestBot::face(Bot &bot, qreal degrees)
{
    bot.lineHeroMouse.setLine(bot.x(), bot.y(), bot.x() + 100 * qCos(qDegreesToRadians(degrees)), bot.y() - 100 * qSin(qDegreesToRadians(degrees)));
}

QPointF TestBot::from(QPointF point, qreal degrees, qreal distance)
{
    return point + distance * QPointF(qCos(qDegreesToRadians(degrees)), -qSin(qDegreesToRadians(degrees)));
}

bool TestBot::notices(Bot &bot, Player *player, QPointF at, qreal facing, QPointF target)
{
    bot.setPos(at);
    face(bot, facing);
    player->setPos(target - QPointF(5, 5));
    bot.checkNearestArea();
    const bool seen = bot.targetAttack == player;
    bot.targetAttack = nullptr;
    bot.STATE_ATTACK = bot.STATE_HIDE = false;
    return seen;
}

void TestBot::noticesLikeThePlayer()
{
    World world;
    Player * player = world.player();
    Bot & bot = *world.addBot(QPointF(700, 700));
    const QPointF at(700, 700), eye = at + QPointF(5, 5);

    QVERIFY(notices(bot, player, at, 0, from(eye, 0, 100)));          // in front
    QVERIFY(!notices(bot, player, at, 0, from(eye, 180, 100)));       // behind (the old bot noticed it)
    QVERIFY(notices(bot, player, at, 0, from(eye, 18, 150)));         // within 20 deg
    QVERIFY(!notices(bot, player, at, 0, from(eye, -26, 150)));
    QVERIFY(notices(bot, player, at, 0, from(eye, 180, 20)));         // near circle
    QVERIFY(!notices(bot, player, at, 0, from(eye, 180, 40)));
    QVERIFY(notices(bot, player, at, 0, eye + QPointF(480, 0)));      // within the screen sideways
    QVERIFY(!notices(bot, player, at, 0, eye + QPointF(560, 0)));     // past its edge
    QVERIFY(notices(bot, player, at, 90, eye + QPointF(0, -250)));    // within the screen upwards
    QVERIFY(!notices(bot, player, at, 90, eye + QPointF(0, -320)));
    QVERIFY(notices(bot, player, QPointF(345, 295), 0, QPointF(385, 300)));     // before the wall
    QVERIFY(!notices(bot, player, QPointF(345, 295), 0, QPointF(470, 300)));    // behind it (the old bot noticed it)
}

void TestBot::noticesChestsOnlyInView()
{
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(700, 700));
    face(bot, 0);
    world.map()->addChest(QPointF(60, 70));          // behind the bot
    bot.checkNearestArea();
    QVERIFY(!bot.goingOpenChest);
    world.map()->addChest(QPointF(80, 70));          // in front
    bot.checkNearestArea();
    QVERIFY(bot.goingOpenChest);
}

void TestBot::searchesTheLastSeenSpot()
{
    World world;
    Player * player = world.player();
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.destinationPoint = QPoint(700, 700);
    face(bot, 0);
    const QPointF spot = from(QPointF(705, 705), 0, 120);
    player->setPos(spot - QPointF(5, 5));
    bot.nextMove();
    QVERIFY(bot.chasing);
    QVERIFY(QLineF(bot.lastSeenTarget, spot).length() < 1);

    bot.stopShooting();
    player->setPos(100, 1400);                       // gone
    bot.nextMove();
    QVERIFY(bot.headingToLastSeen);
    QVERIFY(QLineF(bot.destinationPoint, spot - QPointF(5, 5)).length() < 2);

    int ticks = 0;
    while (bot.lookAroundTicks == 0 && ticks++ < 300)
        bot.nextMove();
    QVERIFY(bot.lookAroundTicks > 0);
    QVERIFY(QLineF(bot.pos(), spot - QPointF(5, 5)).length() <= 3);

    const QPointF standing = bot.pos();
    QSet<int> facings;
    while (bot.lookAroundTicks > 0)
    {
        facings.insert(qRound(bot.lineHeroMouse.angle() / 30) % 12);
        bot.nextMove();
    }
    QCOMPARE(facings.size(), 12);                    // a full circle
    QCOMPARE(bot.pos(), standing);                   // without walking
    QVERIFY(!bot.chasing && !bot.headingToLastSeen);
}

void TestBot::searchIsCancelledBySeeingThePlayer()
{
    World world;
    Player * player = world.player();
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.destinationPoint = QPoint(700, 700);
    face(bot, 0);
    player->setPos(from(QPointF(705, 705), 0, 120) - QPointF(5, 5));
    bot.nextMove();
    bot.stopShooting();
    player->setPos(100, 1400);
    bot.nextMove();
    QVERIFY(bot.headingToLastSeen);

    world.map()->addChest(QPointF(78, 72));          // in view, on the way
    bot.nextMove();
    QVERIFY(!bot.goingOpenChest);                    // ignored while searching
    QVERIFY(bot.headingToLastSeen);

    player->setPos(bot.mapToScene(bot.rect().center()) + QPointF(55, -5));
    face(bot, 0);
    bot.nextMove();
    bot.stopShooting();
    QVERIFY(bot.chasing);
    QVERIFY(!bot.headingToLastSeen);
    QCOMPARE(bot.lookAroundTicks, 0);
}

void TestBot::givesUpOnAChestThatIsGone()
{
    // it used to wait at the spot forever when the chest had been taken
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(600, 600));
    bot.goingOpenChest = true;
    bot.destinationPoint = QPoint(600, 600);
    bot.botIsCloseToDestinationPoint();
    QVERIFY(!bot.goingOpenChest);
    QVERIFY(bot.destinationPoint != QPoint(600, 600));

    world.map()->addChest(QPointF(60, 61));          // still there this time: opened and removed
    bot.goingOpenChest = true;
    bot.destinationPoint = QPoint(600, 600);
    bot.botIsCloseToDestinationPoint();
    QVERIFY(!bot.goingOpenChest);
    QCOMPARE(chestsIn(world.scene()), 0);
}

void TestBot::opensTheChestAtTheEndOfARoute()
{
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(600, 600));
    world.map()->addChest(QPointF(60, 61));
    bot.goingOpenChest = true;
    bot.pathFromAlgorithm = true;
    bot.path = {QPoint(600, 600)};
    bot.destinationPoint = QPoint(600, 600);
    bot.botIsCloseToDestinationPoint();
    QVERIFY(!bot.goingOpenChest);
    QVERIFY(!bot.pathFromAlgorithm);
    QCOMPARE(chestsIn(world.scene()), 0);
}

void TestBot::routesToAChestBehindAWallOnce()
{
    // the bot used to aim inside the chest (unreachable) and search again every tick, cutting through corners
    World world;
    world.player()->setPos(1400, 1400);
    Bot & bot = *world.addBot(QPointF(380, 320));
    world.map()->addChest(QPointF(45, 32));          // x 450..470, y 320..330: right behind the wall
    Chest * chest = nullptr;
    for (QGraphicsItem * item : world.scene()->items())
        if (Chest * c = dynamic_cast<Chest *>(item))
            chest = c;
    bot.destinationPoint = bot.freeCellNextToChest(chest);
    QVERIFY(!world.map()->checkIfPointIsTaken(bot.destinationPoint / 10));
    bot.goingOpenChest = true;

    int routes = 0, ticks = 0;
    const void * lastPath = bot.path.constData();
    for (; ticks < 1500 && chestsIn(world.scene()) > 0; ticks++)
    {
        bot.nextMove();
        if (bot.path.constData() != lastPath && !bot.path.isEmpty())
            routes++;
        lastPath = bot.path.constData();
        for (QGraphicsItem * item : world.scene()->items(QRectF(bot.x() + 1, bot.y() + 1, 8, 8)))
            QVERIFY2(!dynamic_cast<Building *>(item), qPrintable(QString("inside a building at tick %1").arg(ticks)));
    }
    QCOMPARE(chestsIn(world.scene()), 0);
    QVERIFY2(routes <= 3, qPrintable(QString("%1 routes computed").arg(routes)));
}

void TestBot::deathClearsRouteAndMemory()
{
    // a respawned bot used to follow its old route through buildings
    World world;
    Player * player = world.player();
    player->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    bot.pathFromAlgorithm = true;
    bot.goingOpenChest = true;
    bot.path = {QPoint(100, 100), QPoint(110, 100)};
    bot.chasing = bot.headingToLastSeen = true;
    bot.attackItem(1000, player);
    QVERIFY(!bot.isAlive());
    QVERIFY(bot.path.isEmpty() && !bot.pathFromAlgorithm && !bot.goingOpenChest);
    QVERIFY(!bot.chasing && !bot.headingToLastSeen && bot.lookAroundTicks == 0);
    QCOMPARE(player->getHeroStats()->getKills(), 1u);
}

// the bot with the player 200 px in front of it, in view, then gone: the last seen spot is `spot`
void TestBot::loseSightOfThePlayer(World & world, Bot & bot, QPointF & spot)
{
    Player * player = world.player();
    bot.destinationPoint = QPoint(700, 700);
    bot.lineHeroMouse.setLine(bot.x(), bot.y(), bot.x() + 100, bot.y());
    spot = QPointF(905, 705);
    player->setPos(spot - QPointF(5, 5));
    bot.nextMove();
    bot.stopShooting();
    player->setPos(100, 1400);
    bot.nextMove();
}

void TestBot::throwsAGrenadeWhereItLostSightOfThePlayer()
{
    World world;
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    QPointF spot;
    loseSightOfThePlayer(world, bot, spot);
    QVERIFY(bot.ownGrenade);
    QCOMPARE(bot.getGrenades(), 2);
    // thrown at the spot, ~200 px to the right (within a degree: the bot may have stepped 2 px meanwhile)
    QVERIFY(bot.ownGrenade->dx > 0 && qAbs(bot.ownGrenade->dy / bot.ownGrenade->dx) < 0.02);
    QVERIFY(bot.headingToLastSeen);                      // and it still goes to look
}

void TestBot::throwsAGrenadeWhileReloading()
{
    World world;
    Player * player = world.player();
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    bot.destinationPoint = QPoint(700, 700);
    face(bot, 0);
    bot.gun()->decreaseAmmo();
    bot.gun()->reload();
    QVERIFY(bot.gun()->isReloading());
    player->setPos(QPointF(905, 705) - QPointF(5, 5));
    bot.nextMove();
    QVERIFY(bot.ownGrenade);
    QCOMPARE(bot.getGrenades(), 2);
}

void TestBot::grenadeLimits()
{
    World world;
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    const QPointF me = bot.centre();
    QVERIFY(!bot.throwGrenadeAt(me + QPointF(60, 0)));   // it would be caught in the blast
    QVERIFY(!bot.throwGrenadeAt(me + QPointF(450, 0)));  // it would go off in the air
    QVERIFY(bot.throwGrenadeAt(me + QPointF(200, 0)));
    QVERIFY(!bot.throwGrenadeAt(me + QPointF(0, 200)));  // one at a time
    delete bot.ownGrenade;
    QVERIFY(!bot.throwGrenadeAt(me + QPointF(0, 200)));  // and 6 s apart
    bot.grenadeCooldownTicks = 0;
    QVERIFY(bot.throwGrenadeAt(me + QPointF(0, 200)));
    delete bot.ownGrenade;
    bot.grenadeCooldownTicks = 0;
    QVERIFY(bot.throwGrenadeAt(me + QPointF(-200, 0)));
    delete bot.ownGrenade;
    bot.grenadeCooldownTicks = 0;
    QCOMPARE(bot.getGrenades(), 0);
    QVERIFY(!bot.throwGrenadeAt(me + QPointF(200, 0)));  // none left
}

void TestBot::grenadeLandsWhereItWasAimed()
{
    // a real throw in real time on open ground
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(300, 1200));
    bot.start();                                         // it may walk off meanwhile; the target stays where it was
    for (qreal distance : {280.0})
    {
        const QPointF target = bot.centre() + QPointF(distance, 0);
        QVERIFY(bot.throwGrenadeAt(target));
        QPointer<Grenade> grenade = bot.ownGrenade;
        QPointF landed;
        for (int t = 0; t < 300 && grenade; t++)        // the fuse is 5 s
        {
            if (grenade->moveTimer.isActive() == false && landed.isNull())
                landed = grenade->pos();
            runFor(20);
        }
        QVERIFY2(!landed.isNull(), "it never landed");
        QVERIFY2(QLineF(landed, target).length() < 15,
                 qPrintable(QString("aimed %1 px, landed %2 px from the target").arg(distance).arg(QLineF(landed, target).length())));
        QVERIFY(!grenade);                               // and went off
    }
}

void TestBot::waitsForItsOwnGrenadeAndStepsAwayFromIt()
{
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    Grenade * grenade = new Grenade(&bot, 0);
    grenade->moveTimer.stop();
    world.scene()->addItem(grenade);
    bot.ownGrenade = grenade;

    grenade->setPos(800, 705);                           // lying where the bot wants to go
    bot.destinationPoint = QPoint(795, 700);
    for (int i = 0; i < 20; i++)
        bot.nextMove();
    QCOMPARE(bot.pos(), QPointF(700, 700));              // waits

    grenade->setPos(bot.centre() + QPointF(20, 0));      // lying right next to it
    for (int i = 0; i < 40; i++)
        bot.nextMove();
    QVERIFY2(QLineF(bot.centre(), grenade->pos()).length() >= bot.GRENADE_DANGER,
             qPrintable(QString("still %1 px away").arg(QLineF(bot.centre(), grenade->pos()).length())));

    delete grenade;                                      // gone off: carries on to where it was going
    bot.destinationPoint = QPoint(795, 700);
    bot.path.clear();
    bot.pathFromAlgorithm = false;
    const QPointF before = bot.pos();
    bot.nextMove();
    QVERIFY(bot.pos() != before);
}

// a grenade of the player's lying still at `at`
static Grenade * lyingGrenade(World & world, QPointF at)
{
    Grenade * grenade = new Grenade(world.player(), 0);
    world.scene()->addItem(grenade);
    grenade->setPos(at);
    return grenade;
}

void TestBot::dodgesAGrenadeItSees()
{
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    bot.destinationPoint = QPoint(1000, 700);            // walking right, so facing right
    face(bot, 0);
    Grenade * grenade = lyingGrenade(world, bot.centre() + QPointF(45, 0));
    grenade->moveTimer.stop();
    for (int i = 0; i < 40; i++)
        bot.nextMove();
    QCOMPARE(bot.seenGrenades.size(), 1);
    QVERIFY2(QLineF(bot.centre(), grenade->pos()).length() >= bot.GRENADE_DANGER,
             qPrintable(QString("still %1 px away").arg(QLineF(bot.centre(), grenade->pos()).length())));

    delete grenade;                                      // gone off: forgotten
    bot.nextMove();
    QVERIFY(bot.seenGrenades.isEmpty());
}

void TestBot::ignoresAGrenadeItDoesNotSee()
{
    World world;
    world.player()->setPos(100, 1400);
    Bot & bot = *world.addBot(QPointF(700, 700));
    bot.start();
    bot.destinationPoint = QPoint(1000, 700);
    face(bot, 0);
    Grenade * grenade = lyingGrenade(world, bot.centre() + QPointF(-45, 0));   // behind it, outside the near circle
    grenade->moveTimer.stop();
    for (int i = 0; i < 10; i++)
        bot.nextMove();
    QVERIFY(bot.seenGrenades.isEmpty());
    QCOMPARE(bot.destinationPoint, QPoint(1000, 700));    // carries on as it was
    QCOMPARE(bot.pos(), QPointF(720, 700));
}

int runTestBot(int argc, char *argv[])
{
    TestBot test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_bot.moc"
