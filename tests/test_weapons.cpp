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

int runTestWeapons(int argc, char *argv[])
{
    TestWeapons test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_weapons.moc"
