#include <QtTest>
#include <QKeyEvent>

#include "tests/helpers.h"
#include "weapons/sniper.h"

// the sniper rifle and its scope; (700,700) is open ground
class TestSniper : public QObject
{
    Q_OBJECT

private slots:
    void cleanup() { GameClock::reset(); }

    void stats();
    void oneShotEveryOneAndAHalfSeconds();
    void scopeOnlyWithTheSniper();
    void heroStandsStillWhileScoped();
    void theModeIsShown();
    void shiftMovesTheScreenNotTheHero();
    void aBuildingStopsTheScreenWhichSlidesAlongIt();
    void theMapEdgeStopsTheScreenThenTheCursorGoesOn();
    void shiftKeepsTheMouseUntilLetGo();
    void lookingIsLimitedToOneScreenWidth();
    void shiftOverAHiddenSpotStartsAtTheWall();
    void everyFreeDirectionWorksAfterTouchingABuilding();
    void theTargetNeverGoesOntoOrBehindABuilding();
    void leavingTheScopeBringsTheScreenBack();

private:
    static void key(QWidget * widget, int key, QEvent::Type type = QEvent::KeyPress);
    // the player at `at` with the camera on it and the cursor in the middle of the screen
    static Player * ready(World & world, QPointF at);
    static void scope(World & world);
};

void TestSniper::key(QWidget *widget, int key, QEvent::Type type)
{
    QKeyEvent event(type, key, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

Player * TestSniper::ready(World &world, QPointF at)
{
    Player * player = world.player();
    player->start();
    player->setPos(at);
    emit player->posChanged();
    emit world.view()->sendCord(world.view()->viewport()->rect().center());
    return player;
}

void TestSniper::scope(World &world)
{
    key(&world.mapWidget, Qt::Key_4);
    emit world.view()->rightClicked();
}

void TestSniper::stats()
{
    Sniper sniper;
    QCOMPARE(sniper.getDamage(), 12);
    QCOMPARE(sniper.ammoLoaded(), 5);
    QCOMPARE(sniper.ammoOwned(), 20);
    QCOMPARE(sniper.getBulletSpeed(), 10);                // twice the others
    QVERIFY(!sniper.isAutomatic());

    World world;
    Bot * bot = world.addBot(QPointF(700, 700));
    bot->start();
    bot->attackItem(sniper.getDamage(), world.player());
    QVERIFY(bot->isAlive());
    bot->attackItem(sniper.getDamage(), world.player());
    QVERIFY(!bot->isAlive());                             // two hits
}

void TestSniper::oneShotEveryOneAndAHalfSeconds()
{
    World world;
    Player * player = ready(world, QPointF(700, 700));
    key(&world.mapWidget, Qt::Key_4);
    QCOMPARE(player->gunENUM(), SNIPER);
    player->startShooting();
    player->stopShooting();
    QCOMPARE(player->gun()->ammoLoaded(), 4);
    runFor(300);
    player->startShooting();                              // too soon
    player->stopShooting();
    QCOMPARE(player->gun()->ammoLoaded(), 4);
    runFor(1300);
    player->startShooting();
    player->stopShooting();
    QCOMPARE(player->gun()->ammoLoaded(), 3);

    key(&world.mapWidget, Qt::Key_1);                     // the pistol has no such limit
    player->startShooting();
    player->stopShooting();
    runFor(250);
    player->startShooting();
    player->stopShooting();
    QCOMPARE(player->gun()->ammoLoaded(), 10);
}

void TestSniper::scopeOnlyWithTheSniper()
{
    World world;
    Player * player = ready(world, QPointF(700, 700));
    emit world.view()->rightClicked();                    // the rifle
    QVERIFY(!player->isScoped());
    scope(world);
    QVERIFY(player->isScoped());
    emit world.view()->rightClicked();                    // right click again: out
    QVERIFY(!player->isScoped());
    scope(world);
    key(&world.mapWidget, Qt::Key_2);                     // another gun: out
    QVERIFY(!player->isScoped());
    scope(world);
    player->attackItem(1000, nullptr);                    // dying: out
    QVERIFY(!player->isScoped());
}

void TestSniper::heroStandsStillWhileScoped()
{
    World world;
    Player * player = ready(world, QPointF(700, 700));
    key(&world.mapWidget, Qt::Key_D);                     // walking right...
    runFor(100);
    scope(world);                                         // ...stops
    const QPointF at = player->pos();
    key(&world.mapWidget, Qt::Key_W);
    runFor(200);
    QCOMPARE(player->pos(), at);
    key(&world.mapWidget, Qt::Key_D, QEvent::KeyRelease);
    key(&world.mapWidget, Qt::Key_W, QEvent::KeyRelease);
    emit world.view()->rightClicked();
    key(&world.mapWidget, Qt::Key_W);                     // out of the scope it walks again
    runFor(100);
    QVERIFY(player->y() < at.y());
}

static bool near(QPointF a, QPointF b) { return QLineF(a, b).length() <= 1.5; }   // the cursor sits on a whole pixel

// pans in mouse-sized steps; returns the largest jump of the point looked at in one step
static qreal pan(MapWidget & mw, QPointF step, int times)
{
    qreal largest = 0;
    for (int i = 0; i < times; i++)
    {
        const QPointF before = mw.getLookPoint();
        mw.panScope(step);
        largest = qMax(largest, QLineF(before, mw.getLookPoint()).length());
    }
    return largest;
}

void TestSniper::theModeIsShown()
{
    World world;
    ready(world, QPointF(700, 700));
    MapWidget & mw = world.mapWidget;
    QWidget * viewport = world.view()->viewport();
    QVERIFY(mw.getScopeHint().isEmpty());
    scope(world);
    QVERIFY(mw.getScopeHint().contains("hold Shift"));
    QCOMPARE(viewport->cursor().shape(), Qt::CrossCursor);
    key(&mw, Qt::Key_Shift);
    QVERIFY(mw.getScopeHint().contains("moving the view"));
    QCOMPARE(viewport->cursor().shape(), Qt::SizeAllCursor);
    key(&mw, Qt::Key_Shift, QEvent::KeyRelease);
    QVERIFY(mw.getScopeHint().contains("hold Shift"));
    QCOMPARE(viewport->cursor().shape(), Qt::CrossCursor);
    emit world.view()->rightClicked();
    QVERIFY(mw.getScopeHint().isEmpty());
    QCOMPARE(viewport->cursor().shape(), Qt::ArrowCursor);
}

void TestSniper::shiftMovesTheScreenNotTheHero()
{
    World world;
    Player * player = ready(world, QPointF(700, 700));
    MapWidget & mw = world.mapWidget;
    scope(world);
    const QPointF hero = player->centre();
    const QPointF cameraBefore = world.view()->sceneRect().center();

    mw.panScope(QPointF(300, 0));                         // without Shift the mouse only moves the cursor
    QCOMPARE(world.view()->sceneRect().center(), cameraBefore);

    key(&mw, Qt::Key_Shift);
    const QPointF start = mw.getLookPoint();              // under the cursor, in the middle of the screen
    QVERIFY(near(start, hero));
    QVERIFY(pan(mw, QPointF(10, -5), 20) <= 11.2);        // follows the mouse step by step
    QCOMPARE(player->centre(), hero);                     // the hero stays
    QCOMPARE(mw.getLookPoint(), start + QPointF(200, -100));
    QVERIFY(near(world.view()->sceneRect().center(), cameraBefore + QPointF(200, -100)));
    QVERIFY(qAbs(player->lineAngle() - QLineF(hero, start + QPointF(200, -100)).angle()) < 0.5);   // aims there
    const QPointF camera = world.view()->sceneRect().center();
    key(&mw, Qt::Key_Shift, QEvent::KeyRelease);
    QCOMPARE(world.view()->sceneRect().center(), camera); // stays where it was moved
}

// the player 100 px right of the building at x 820..860, y 790..830 (its outline ends at x 860.5), where the screen
// can follow the look both ways
static const QPointF RIGHT_OF_WALL(955, 805);           // centre (960,810)

void TestSniper::aBuildingStopsTheScreenWhichSlidesAlongIt()
{
    World world;
    Player * player = ready(world, RIGHT_OF_WALL);
    MapWidget & mw = world.mapWidget;
    scope(world);
    mw.setPanning(true);
    QVERIFY(pan(mw, QPointF(-10, 0), 30) <= 10.01);      // towards the wall: no jumps
    const QPointF atWall = mw.getLookPoint();
    QVERIFY2(atWall.x() >= 860.5 && atWall.x() < 871, qPrintable(QString::number(atWall.x())));   // stopped before it
    QVERIFY(QLineF(player->centre(), atWall).length() < 100);

    // down and left: it can't go left, so it slides down until the line from the hero passes below the building,
    // and then goes left as well
    QVERIFY(pan(mw, QPointF(-10, 10), 15) <= 14.2);
    QVERIFY(mw.getLookPoint().y() > atWall.y() + 100);
    QVERIFY2(mw.getLookPoint().x() < atWall.x() - 20, qPrintable(QString::number(mw.getLookPoint().x())));
    QVERIFY(QLineF(player->centre(), mw.getLookPoint()).length() > 150);   // no distance lost on the way
}

void TestSniper::theMapEdgeStopsTheScreenThenTheCursorGoesOn()
{
    World world;
    Player * player = ready(world, QPointF(700, 700));
    MapWidget & mw = world.mapWidget;
    scope(world);
    mw.setPanning(true);
    pan(mw, QPointF(20, 0), 50);                          // 1000 px of mouse to the right
    const QRectF shown = world.view()->mapToScene(world.view()->viewport()->rect()).boundingRect();
    QVERIFY(shown.right() <= MAP_SIZE_X_FULL + 1 && shown.right() >= MAP_SIZE_X_FULL - 1);   // the screen at the edge
    QVERIFY2(mw.getLookPoint().x() > MAP_SIZE_X_FULL - 21 && mw.getLookPoint().x() <= MAP_SIZE_X_FULL - 1,
             qPrintable(QString::number(mw.getLookPoint().x())));                          // the target went on to it
    QVERIFY((world.view()->mapFromScene(mw.getLookPoint()) - mw.cursorAt).manhattanLength() <= 1);   // under the cursor
    QVERIFY(world.view()->viewport()->rect().contains(mw.cursorAt));                             // still on the view
    const QPointF look = mw.getLookPoint();
    mw.panScope(QPointF(-20, 0));                         // straight back
    QCOMPARE(mw.getLookPoint(), look - QPointF(20, 0));
    QCOMPARE(player->centre(), QPointF(705, 705));

    // all four edges: the cursor never leaves the view, and it comes back from each
    for (QPointF step : {QPointF(0, -20), QPointF(-20, 0), QPointF(0, 20), QPointF(20, 0)})
    {
        pan(mw, step, 80);
        QVERIFY(world.view()->viewport()->rect().contains(mw.cursorAt));
        const QPointF atEdge = mw.getLookPoint();
        mw.panScope(-step);
        QCOMPARE(mw.getLookPoint(), atEdge - step);
    }
}

void TestSniper::shiftKeepsTheMouseUntilLetGo()
{
    // so a quick flick out of the window still moves the view
    World world;
    ready(world, QPointF(700, 700));
    MapWidget & mw = world.mapWidget;
    scope(world);
    QVERIFY(!QWidget::mouseGrabber());
    key(&mw, Qt::Key_Shift);
    QCOMPARE(QWidget::mouseGrabber(), world.view()->viewport());
    key(&mw, Qt::Key_Shift, QEvent::KeyRelease);
    QVERIFY(!QWidget::mouseGrabber());
    key(&mw, Qt::Key_Shift);
    emit world.view()->rightClicked();                    // leaving the scope lets go too
    QVERIFY(!QWidget::mouseGrabber());
}

void TestSniper::lookingIsLimitedToOneScreenWidth()
{
    World world;
    Player * player = ready(world, QPointF(700, 195));    // near the top: 1000 px down is still on the map
    MapWidget & mw = world.mapWidget;
    scope(world);
    mw.setPanning(true);
    pan(mw, QPointF(0, 20), 100);
    const qreal distance = QLineF(player->centre(), mw.getLookPoint()).length();
    QVERIFY2(distance <= MapWidget::MAX_LOOK_DISTANCE && distance > MapWidget::MAX_LOOK_DISTANCE - 20,
             qPrintable(QString::number(distance)));
    const QPointF look = mw.getLookPoint();
    mw.panScope(QPointF(0, -20));                         // back at once
    QCOMPARE(mw.getLookPoint(), look - QPointF(0, 20));
}

// the player 100 px left of the building at x 820..860, y 790..830
static const QPointF LEFT_OF_WALL(715, 805);            // centre (720,810)

void TestSniper::shiftOverAHiddenSpotStartsAtTheWall()
{
    World world;
    ready(world, LEFT_OF_WALL);
    MapWidget & mw = world.mapWidget;
    scope(world);
    emit world.view()->sendCord(world.view()->viewport()->rect().center() + QPoint(200, 0));   // behind the building
    mw.setPanning(true);
    const QPointF start = mw.getLookPoint();
    QVERIFY2(start.x() > 810 && start.x() < 820, qPrintable(QString::number(start.x())));   // in front of it instead
    QVERIFY((world.view()->mapFromScene(start) - mw.cursorAt).manhattanLength() <= 1);   // the cursor went there
    mw.panScope(QPointF(5, 0));                           // into the building: no
    QCOMPARE(mw.getLookPoint(), start);
    for (QPointF step : {QPointF(0, -5), QPointF(0, 5), QPointF(-5, 0)})   // every other way: yes
    {
        const QPointF before = mw.getLookPoint();
        mw.panScope(step);
        QCOMPARE(mw.getLookPoint(), before + step);
    }
}

void TestSniper::everyFreeDirectionWorksAfterTouchingABuilding()
{
    World world;
    ready(world, RIGHT_OF_WALL);
    MapWidget & mw = world.mapWidget;
    scope(world);
    mw.setPanning(true);
    pan(mw, QPointF(-3, 0), 60);                          // against the wall
    QVERIFY(mw.getLookPoint().x() < 864);
    for (QPointF step : {QPointF(0, -3), QPointF(0, 3), QPointF(3, 0), QPointF(3, 3), QPointF(3, -3), QPointF(-3, 3)})
    {
        const QPointF before = mw.getLookPoint();
        mw.panScope(step);
        QVERIFY2(mw.getLookPoint() != before, qPrintable(QString("stuck going %1,%2").arg(step.x()).arg(step.y())));
    }
}

void TestSniper::theTargetNeverGoesOntoOrBehindABuilding()
{
    // a long random walk of small mouse steps around the building
    World world;
    Player * player = ready(world, RIGHT_OF_WALL);
    MapWidget & mw = world.mapWidget;
    scope(world);
    mw.setPanning(true);
    pan(mw, QPointF(-4, 0), 20);
    QRandomGenerator random(1234);
    const QRectF building(820, 790, 40, 40);
    for (int i = 0; i < 3000; i++)
    {
        const QPointF step(random.bounded(-6, 7) - (i % 200 < 100 ? 1 : -1), random.bounded(-6, 7));
        const QPointF before = mw.getLookPoint();
        mw.panScope(step);
        const QPointF look = mw.getLookPoint();
        QVERIFY2(!building.contains(look), qPrintable(QString("on the building at step %1").arg(i)));
        const qreal distance = QLineF(player->centre(), look).length();
        QVERIFY2(mw.getFieldOfView().clearDistance(player->centre(), look) >= distance - 0.02,
                 qPrintable(QString("behind the building at step %1: %2,%3").arg(i).arg(look.x()).arg(look.y())));
        QVERIFY(QLineF(before, look).length() <= 9.3);   // never more than the step (at most 7 by 6): no jumps
    }
}

void TestSniper::leavingTheScopeBringsTheScreenBack()
{
    World world;
    Player * player = ready(world, QPointF(700, 700));
    MapWidget & mw = world.mapWidget;
    const QPointF cameraBefore = world.view()->sceneRect().center();
    scope(world);
    mw.setPanning(true);
    mw.panScope(QPointF(-250, 200));
    QVERIFY(world.view()->sceneRect().center() != cameraBefore);
    emit world.view()->rightClicked();
    QVERIFY(!player->isScoped());
    QVERIFY(!mw.isPanning());
    QCOMPARE(world.view()->sceneRect().center(), cameraBefore);
    QCOMPARE(player->viewCentre(), player->centre());
}

int runTestSniper(int argc, char *argv[])
{
    TestSniper test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_sniper.moc"
