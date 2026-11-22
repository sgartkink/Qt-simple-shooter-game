#include <QtTest>
#include <QPointer>

#include "tests/helpers.h"
#include "weapons/rifle.h"
#include "weapons/grenadeellipseitem.h"

class TestWeapons : public QObject
{
    Q_OBJECT

private slots:
    void ammoThatDoesNotFitGoesToTheReserve();
    void reserveIsCappedAtMaximum();
    void ammoThatFitsStaysInTheMagazine();
    void refillsDoWhatTheirNamesSay();
    void bulletsHitOnFirstContact();
    void bulletsStopAtWallsUnderExplosions();
    void bulletsIgnoreTheirShooter();
    void grenadeRangeMatchesThePower();
    void grenadesBounceOffHeroesFlyingLowerThanThem();
    void grenadesComingDownOnSomethingBounceBack();

private:
    QRectF fire(World & world, QPointF start, qreal degrees);
};

void TestWeapons::ammoThatDoesNotFitGoesToTheReserve()
{
    Rifle rifle;                                          // 30 per magazine, 210 reserve at most
    for (int i = 0; i < 30; i++)
        rifle.decreaseAmmo();
    rifle.reload();                                       // 30 loaded, 180 owned
    for (int i = 0; i < 5; i++)
        rifle.decreaseAmmo();                             // 25 loaded
    rifle.addAmmo(20);
    QCOMPARE(rifle.ammoLoaded(), 30);
    QCOMPARE(rifle.ammoOwned(), 195);
}

void TestWeapons::reserveIsCappedAtMaximum()
{
    Rifle rifle;
    for (int i = 0; i < 5; i++)
        rifle.decreaseAmmo();                             // 25 loaded, 210 owned (full)
    rifle.addAmmo(20);
    QCOMPARE(rifle.ammoLoaded(), 30);
    QCOMPARE(rifle.ammoOwned(), 210);
}

void TestWeapons::ammoThatFitsStaysInTheMagazine()
{
    Rifle rifle;
    for (int i = 0; i < 25; i++)
        rifle.decreaseAmmo();
    rifle.addAmmo(20);
    QCOMPARE(rifle.ammoLoaded(), 25);
    QCOMPARE(rifle.ammoOwned(), 210);
}

void TestWeapons::refillsDoWhatTheirNamesSay()
{
    // the two refills used to call each other's Gun method
    World world;
    Player * player = world.player();
    for (int i = 0; i < 10; i++)
        player->gun()->decreaseAmmo();
    player->gun()->reload();                              // 30 loaded, 200 owned
    for (int i = 0; i < 10; i++)
        player->gun()->decreaseAmmo();                    // 20 loaded
    player->refillCurrentLoadedAmmo();
    QCOMPARE(player->gun()->ammoLoaded(), 30);
    QCOMPARE(player->gun()->ammoOwned(), 200);
    player->refillCurrentOwnedAmmo();
    QCOMPARE(player->gun()->ammoOwned(), 210);
}

// fires one bullet from `start` at `degrees` (0 = right, 90 = up), stepping it by hand;
// returns the bullet's drawn rectangle at the moment it hit something, or an empty rect if it never did
QRectF TestWeapons::fire(World & world, QPointF start, qreal degrees)
{
    Player * shooter = world.player();
    const QPointF direction(qCos(qDegreesToRadians(degrees)), -qSin(qDegreesToRadians(degrees)));
    const QPointF shooterCentre = shooter->mapToScene(shooter->rect().center());
    shooter->updateLineHeroMouse(world.view()->mapFromScene(shooterCentre + direction * 100));

    QPointer<Bullet> bullet = new Bullet(shooter, 1, 5 * direction.x(), 5 * direction.y());
    bullet->moveTimer.stop();
    bullet->setPos(start);
    world.scene()->addItem(bullet);
    for (int i = 0; i < 100; i++)
    {
        QRectF drawn = bullet->mapToScene(bullet->rect()).boundingRect();
        bullet->move();
        if (!bullet)
            return drawn.translated(direction * 5);
    }
    delete bullet;
    return QRectF();
}

void TestWeapons::bulletsHitOnFirstContact()
{
    // the hit used to come up to 2 px before contact, depending on the direction
    World world;
    world.player()->setPos(1400, 1400);
    Player target(nullptr, &world.mapWidget);
    target.setPos(700, 700);
    world.scene()->addItem(&target);
    const QRectF t(700, 700, 10, 10);

    struct Shot { QPointF start; QPointF step; qreal degrees; };
    const Shot shots[] = { {{600, 704}, {1, 0}, 0}, {{800, 704}, {1, 0}, 180}, {{704, 600}, {0, 1}, 270}, {{704, 800}, {0, 1}, 90} };
    for (const Shot & shot : shots)
        for (int offset = 0; offset < 5; offset++)
        {
            QRectF hit = fire(world, shot.start + shot.step * offset, shot.degrees);
            QVERIFY2(!hit.isNull(), "the bullet missed");
            const qreal gap = shot.degrees == 0 ? t.left() - hit.right() : shot.degrees == 180 ? hit.left() - t.right()
                            : shot.degrees == 270 ? t.top() - hit.bottom() : hit.top() - t.bottom();
            // first touching tick: overlapping (gap <= the 0.5 px pen) but by less than one 5 px step
            QVERIFY2(gap <= 0.5 && gap > -5.5, qPrintable(QString("direction %1, offset %2: gap %3").arg(shot.degrees).arg(offset).arg(gap)));
        }
    world.scene()->removeItem(&target);
}

void TestWeapons::bulletsStopAtWallsUnderExplosions()
{
    // with itemAt(), an explosion on top used to hide the wall
    World world;
    world.player()->setPos(1400, 1400);
    GrenadeEllipseItem * explosion = new GrenadeEllipseItem();
    explosion->setRect(-40, -40, 80, 80);
    explosion->setPos(420, 300);
    world.scene()->addItem(explosion);
    QRectF hit = fire(world, QPointF(350, 300), 0);
    QVERIFY(!hit.isNull());
    QVERIFY(hit.left() < 405);
    delete explosion;
}

void TestWeapons::bulletsIgnoreTheirShooter()
{
    World world;
    world.player()->setPos(900, 900);
    QRectF hit = fire(world, QPointF(905, 905), 0);
    QVERIFY(hit.isNull() || hit.left() > 911);
}

void TestWeapons::grenadeRangeMatchesThePower()
{
    // measured in the game on open ground: power 0 lands 89.6 px away, 10 → 132.5, 25 → 214.5, 50 → 396.9
    const qreal measured[][2] = { {0, 89.6}, {10, 132.5}, {25, 214.5}, {50, 396.9} };
    for (auto m : measured)
        QVERIFY2(qAbs(Grenade::flightDistance(m[0]) - m[1]) < 1.5,
                 qPrintable(QString("power %1: %2 px").arg(m[0]).arg(Grenade::flightDistance(m[0]))));
    for (qreal distance : {100.0, 180.0, 250.0, 390.0})
        QVERIFY(qAbs(Grenade::flightDistance(Grenade::powerFor(distance)) - distance) < 0.1);
    QCOMPARE(Grenade::powerFor(50), -1.0);                // closer than the weakest throw
    QCOMPARE(Grenade::powerFor(2000), -1.0);              // further than the strongest
}

void TestWeapons::grenadesBounceOffHeroesFlyingLowerThanThem()
{
    // heroes are 10 high, like buildings (50) and chests (5) for grenades. The weakest throw lands 90 px away:
    // 40 px out it is ~21 high and flies over a hero; ~79 px out (where it meets a hero standing at 83) it is
    // ~8 high and bounces back.
    // Two throws side by side on open ground, in real time; the throwers' own heroes don't stop their grenades.
    World world;
    Player * low = world.player();
    Player high(world.view(), &world.mapWidget);
    world.scene()->addItem(&high);
    Player wallAt83(world.view(), &world.mapWidget), wallAt40(world.view(), &world.mapWidget);
    world.scene()->addItem(&wallAt83);
    world.scene()->addItem(&wallAt40);

    low->setPos(300, 1100);
    wallAt83.setPos(388, 1100);
    high.setPos(300, 1300);
    wallAt40.setPos(340, 1300);
    QVERIFY(qAbs(wallAt83.getHeight() - 10) < 1e-9);    // it used to be left unset

    Grenade * grenades[2];
    Player * throwers[] = { low, &high };
    for (int i = 0; i < 2; i++)
    {
        const QPointF centre = throwers[i]->sceneBoundingRect().center();
        throwers[i]->updateLineHeroMouse(world.view()->mapFromScene(centre + QPointF(100, 0)));
        QVERIFY(qAbs(throwers[i]->lineAngle()) < 2 || qAbs(throwers[i]->lineAngle() - 360) < 2);
        grenades[i] = throwers[i]->throwGrenade(0, world.view());
    }
    runFor(2700);                                         // both have landed (2.3 s), none has gone off (5 s)
    const qreal lowFlight = grenades[0]->x() - 305, highFlight = grenades[1]->x() - 305;
    QVERIFY2(lowFlight < 78, qPrintable(QString("the low grenade ended %1 px out, past the hero at 83").arg(lowFlight)));
    QVERIFY2(qAbs(highFlight - Grenade::flightDistance(0)) < 5,
             qPrintable(QString("the high grenade ended %1 px out, not %2").arg(highFlight).arg(Grenade::flightDistance(0))));
    delete grenades[0];
    delete grenades[1];
    world.scene()->removeItem(&high);
    world.scene()->removeItem(&wallAt83);
    world.scene()->removeItem(&wallAt40);
}

void TestWeapons::grenadesComingDownOnSomethingBounceBack()
{
    // they used to drift on through whatever they came down on. Two throws at once, in real time:
    // - the weakest throw reaches a hero 75 px out while ~12 high (over its head) and sinks below 10 above it
    // - power 60 passes over the wall at x 400..440 while ~56 high and sinks below 50 above it, ~415; once along the
    //   middle of a building, once along the seam between the two that make up the wall
    World world;
    Player * thrower = world.player();
    Player hero(world.view(), &world.mapWidget), roofThrower(world.view(), &world.mapWidget), seamThrower(world.view(), &world.mapWidget);
    world.scene()->addItem(&hero);
    world.scene()->addItem(&roofThrower);
    world.scene()->addItem(&seamThrower);
    thrower->setPos(300, 1200);
    hero.setPos(380, 1200);
    roofThrower.setPos(25, 335);                         // along the middle of the building at y 320..360
    seamThrower.setPos(25, 315);                         // along y 320, where the two buildings meet

    struct Throw { Player * by; double power; qreal itemLeft; } throws[] = { {thrower, 0, 380}, {&roofThrower, 60, 400}, {&seamThrower, 60, 400} };
    QPointer<Grenade> grenades[3];
    for (int i = 0; i < 3; i++)
    {
        const QPointF centre = throws[i].by->sceneBoundingRect().center();
        throws[i].by->updateLineHeroMouse(world.view()->mapFromScene(centre + QPointF(100, 0)));
        grenades[i] = throws[i].by->throwGrenade(throws[i].power, world.view());
    }
    runFor(4800);                                         // both down (2.3 s, 4.85 s), none gone off (5 s)
    for (int i = 0; i < 3; i++)
    {
        QVERIFY(grenades[i]);
        QVERIFY2(grenades[i]->x() + 4 < throws[i].itemLeft,
                 qPrintable(QString("throw %1 ended at x %2, not back before %3").arg(i).arg(grenades[i]->x()).arg(throws[i].itemLeft)));
        delete grenades[i];
    }
    world.scene()->removeItem(&hero);
    world.scene()->removeItem(&roofThrower);
    world.scene()->removeItem(&seamThrower);
}

int runTestWeapons(int argc, char *argv[])
{
    TestWeapons test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_weapons.moc"
