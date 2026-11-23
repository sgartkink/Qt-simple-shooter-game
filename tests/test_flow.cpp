#include <QtTest>
#include <QComboBox>
#include <QKeyEvent>
#include <QPointer>
#include <QVariantAnimation>

#include "tests/helpers.h"
#include "core/gameclock.h"
#include "core/settings.h"
#include "ui/mainwindow.h"

// the game clock, pausing, the menus and the round
class TestFlow : public QObject
{
    Q_OBJECT

private slots:
    void cleanup() { GameClock::reset(); }

    void pausingTheClockFreezesTimersCallbacksAndAnimations();
    void pausingFreezesTheGame();
    void escPausesAndResumes();
    void backToMenuFreezesTheGame();
    void roundResult();
    void roundEndsWhenTimeIsUp();
    void roundLengthComesFromTheOptions();

private:
    static void key(QWidget * widget, int key, QEvent::Type type = QEvent::KeyPress);
    static QPushButton * button(QWidget * parent, const QString & text);
};

void TestFlow::key(QWidget *widget, int key, QEvent::Type type)
{
    QKeyEvent event(type, key, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

QPushButton * TestFlow::button(QWidget *parent, const QString &text)
{
    for (QPushButton * b : parent->findChildren<QPushButton *>())
        if (b->text() == text)
            return b;
    return nullptr;
}

void TestFlow::pausingTheClockFreezesTimersCallbacksAndAnimations()
{
    QObject context;
    GameTimer timer;
    int ticks = 0;
    connect(&timer, &GameTimer::timeout, [&]{ ticks++; });
    timer.start(50);
    bool called = false;
    GameClock::singleShot(250, &context, [&]{ called = true; });
    QVariantAnimation animation;
    animation.setStartValue(0.0);
    animation.setEndValue(1.0);
    animation.setDuration(600);
    animation.start();
    GameClock::track(&animation);

    runFor(130);
    GameClock::setPaused(true);
    const int ticksAtPause = ticks;
    const qint64 timeAtPause = GameClock::now();
    const int animationAtPause = animation.currentTime();
    QVERIFY(ticksAtPause >= 1);
    runFor(400);
    QCOMPARE(ticks, ticksAtPause);
    QVERIFY(!called);                                     // 250 ms of game time haven't passed
    QCOMPARE(GameClock::now(), timeAtPause);
    QCOMPARE(animation.currentTime(), animationAtPause);
    QVERIFY(timer.isActive());                            // only paused

    GameClock::setPaused(false);
    runFor(70);
    QVERIFY(!called);                                     // ~120 ms of its 250 were left
    runFor(130);
    QVERIFY(called);
    QVERIFY(ticks > ticksAtPause);
    QVERIFY(animation.currentTime() > animationAtPause);

    // a callback whose context is gone is dropped
    bool dropped = true;
    {
        QObject gone;
        GameClock::singleShot(10, &gone, [&]{ dropped = false; });
    }
    runFor(50);
    QVERIFY(dropped);
}

void TestFlow::pausingFreezesTheGame()
{
    World world;
    Player * player = world.player();
    player->start();
    player->setPos(300, 1200);
    Bot * bot = world.addBot(QPointF(700, 1200));
    bot->start();
    Bullet * bullet = new Bullet(bot, 1, 5, 0);
    bullet->setPos(800, 1100);
    world.scene()->addItem(bullet);
    QPointer<Grenade> grenade = bot->throwGrenade(30, world.view());
    player->setDirectionRight(true);
    runFor(200);

    GameClock::setPaused(true);
    const QPointF playerAt = player->pos(), botAt = bot->pos(), bulletAt = bullet->pos(), grenadeAt = grenade->pos();
    runFor(5500);                                         // longer than the grenade's fuse
    QCOMPARE(player->pos(), playerAt);
    QCOMPARE(bot->pos(), botAt);
    QCOMPARE(bullet->pos(), bulletAt);
    QVERIFY(grenade);
    QCOMPARE(grenade->pos(), grenadeAt);

    GameClock::setPaused(false);
    runFor(200);
    QVERIFY(player->x() > playerAt.x());
    QVERIFY(grenade && grenade->pos() != grenadeAt);
    runFor(5200);
    QVERIFY(!grenade);                                    // its 5 s ran out after all
}

void TestFlow::escPausesAndResumes()
{
    MainWindow window;
    window.show();
    window.startProgram();
    runFor(3200);                                         // past the start countdown
    MainWidget * game = window.getMainWidget();
    MapWidget * mapWidget = game->getMapWidget();
    Player * player = mapWidget->getPlayer();
    QVERIFY(player->isAlive());

    key(mapWidget, Qt::Key_D);                            // walking right...
    runFor(100);
    key(mapWidget, Qt::Key_Escape);                       // ...then pause
    QVERIFY(GameClock::isPaused());
    QVERIFY(game->getPauseMenu()->isVisible());
    const QPointF at = player->pos();
    key(mapWidget, Qt::Key_W);                            // ignored while paused
    key(mapWidget, Qt::Key_D, QEvent::KeyRelease);
    runFor(300);
    QCOMPARE(player->pos(), at);
    const int leftAtPause = mapWidget->roundMsLeft();

    key(game->getPauseMenu(), Qt::Key_Escape);            // Esc again resumes
    QVERIFY(!GameClock::isPaused());
    QVERIFY(!game->getPauseMenu()->isVisible());
    runFor(200);
    QCOMPARE(player->pos(), at);                          // the keys were let go at the pause
    QVERIFY(mapWidget->roundMsLeft() < leftAtPause);      // and the round's time runs again

    key(mapWidget, Qt::Key_Escape);
    button(game->getPauseMenu(), "Resume")->click();
    QVERIFY(!GameClock::isPaused());
}

void TestFlow::backToMenuFreezesTheGame()
{
    MainWindow window;
    window.show();
    window.startProgram();
    runFor(3200);
    MainWidget * game = window.getMainWidget();
    MapWidget * mapWidget = game->getMapWidget();
    Player * player = mapWidget->getPlayer();
    player->getHeroStats()->increaseKills();
    QPushButton * start = button(&window, "Start game");
    QVERIFY(start && !button(&window, "Continue")->isVisible());

    key(mapWidget, Qt::Key_Escape);
    button(game->getPauseMenu(), "Back to menu")->click();
    QCoreApplication::processEvents();
    QCOMPARE(window.getMainWidget(), game);               // still there, frozen
    QVERIFY(GameClock::isPaused());
    QVERIFY(!game->isVisible());
    QVERIFY(button(&window, "Continue")->isVisible());
    QCOMPARE(start->text(), QString("New game"));
    const QPointF playerAt = player->pos(), botAt = mapWidget->getBot()->pos();
    const int left = mapWidget->roundMsLeft();
    runFor(400);
    QCOMPARE(player->pos(), playerAt);
    QCOMPARE(mapWidget->getBot()->pos(), botAt);
    QCOMPARE(mapWidget->roundMsLeft(), left);

    window.showOptions();                                 // its round length can't change under it
    QVERIFY(!window.findChild<QComboBox *>()->isEnabled());
    window.backToMenu();
    QCOMPARE(window.getMainWidget(), game);               // the options' Back doesn't end it either

    button(&window, "Continue")->click();
    QVERIFY(game->isVisible());
    QVERIFY(!game->getPauseMenu()->isVisible());
    QVERIFY(!GameClock::isPaused());
    QCOMPARE(player->getHeroStats()->getKills(), 1u);
    runFor(200);
    QVERIFY(mapWidget->roundMsLeft() < left);

    key(mapWidget, Qt::Key_Escape);
    button(game->getPauseMenu(), "Back to menu")->click();
    QCoreApplication::processEvents();
    start->click();                                       // New game: the frozen one goes
    QVERIFY(window.getMainWidget() != game);
    QCOMPARE(window.findChildren<MapWidget *>().size(), 1);
    QVERIFY(!GameClock::isPaused());
    mapWidget = window.getMainWidget()->getMapWidget();
    QCOMPARE(mapWidget->getPlayer()->getHeroStats()->getKills(), 0u);
    int bots = 0;
    for (QGraphicsItem * item : mapWidget->getScene()->items())
        if (dynamic_cast<Bot *>(item))
            bots++;
    QCOMPARE(bots, 1);
    window.showOptions();
    QVERIFY(window.findChild<QComboBox *>()->isEnabled() == false);   // this new one is in progress too
    window.backToMenu();
}

void TestFlow::roundResult()
{
    HeroStats you(nullptr), bot(nullptr);
    you.setName("player");
    bot.setName("bot");
    const QVector<HeroStats *> heroes {&you, &bot};
    QCOMPARE(RoundOverWidget::resultFor(heroes, &you), QString("Draw"));
    you.increaseKills();
    QCOMPARE(RoundOverWidget::resultFor(heroes, &you), QString("You win!"));
    bot.increaseKills();
    bot.increaseKills();
    QCOMPARE(RoundOverWidget::resultFor(heroes, &you), QString("bot wins"));
}

void TestFlow::roundEndsWhenTimeIsUp()
{
    MainWindow window;
    window.show();
    window.startProgram();
    MainWidget * game = window.getMainWidget();
    MapWidget * mapWidget = game->getMapWidget();
    mapWidget->setRoundLength(1500);                      // before the countdown ends and the round starts
    runFor(3000 + 1500 + 300);

    QVERIFY(mapWidget->isRoundOver());
    QVERIFY(GameClock::isPaused());                       // everything stopped
    QVERIFY(game->getRoundOverWidget()->isVisible());
    QVERIFY(game->getRoundOverWidget()->getTitle().startsWith("Time's up - "));
    QVERIFY(game->getRoundOverWidget()->getTable().contains("player"));
    QVERIFY(game->getRoundOverWidget()->getTable().contains("bot"));
    QCOMPARE(game->getPlayerBarWidget()->getTimeLeftText(), QString("0:00"));
    key(mapWidget, Qt::Key_Escape);                       // no pause menu over the result
    QVERIFY(!game->getPauseMenu()->isVisible());

    button(game->getRoundOverWidget(), "Play again")->click();
    QCoreApplication::processEvents();
    QVERIFY(window.getMainWidget() && window.getMainWidget() != game);
    QVERIFY(!GameClock::isPaused());
    QVERIFY(!window.getMainWidget()->getMapWidget()->isRoundOver());

    // a finished round can't be continued: back to the menu ends it
    game = window.getMainWidget();
    game->getMapWidget()->setRoundLength(100);
    runFor(3000 + 100 + 300);
    QVERIFY(game->getRoundOverWidget()->isVisible());
    button(game->getRoundOverWidget(), "Back to menu")->click();
    QCoreApplication::processEvents();
    QVERIFY(!window.getMainWidget());
    QVERIFY(!button(&window, "Continue")->isVisible());
    QCOMPARE(button(&window, "New game"), nullptr);
    QVERIFY(!GameClock::isPaused());
    window.showOptions();
    QVERIFY(window.findChild<QComboBox *>()->isEnabled());
}

void TestFlow::roundLengthComesFromTheOptions()
{
    MainWindow window;
    QComboBox * box = window.findChild<QComboBox *>();
    QVERIFY(box);
    QCOMPARE(box->count(), 3);
    QCOMPARE(box->currentData().toInt(), Settings::roundMinutes());

    box->setCurrentIndex(box->findData(5));
    QCOMPARE(Settings::roundMinutes(), 5);
    window.startProgram();
    QCOMPARE(window.getMainWidget()->getPlayerBarWidget()->getTimeLeftText(), QString("5:00"));
    QCOMPARE(window.getMainWidget()->getMapWidget()->roundMsLeft(), 5 * 60000);

    box->setCurrentIndex(box->findData(1));
    window.startProgram();
    QCOMPARE(window.getMainWidget()->getPlayerBarWidget()->getTimeLeftText(), QString("1:00"));

    Settings::setRoundMinutes(7);                         // not a choice: ignored
    QCOMPARE(Settings::roundMinutes(), 1);
    Settings::setRoundMinutes(Settings::DEFAULT_ROUND_MINUTES);
}

int runTestFlow(int argc, char *argv[])
{
    TestFlow test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_flow.moc"
