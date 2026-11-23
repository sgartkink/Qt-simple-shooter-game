#include "ui/mainwidget.h"
#include "core/global_consts.h"
#include <QtDebug>
MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    playerBarWidget->setMinimumHeight(100);

    countdown = new Countdown(this);
    countdown->move(0, (WINDOW_HEIGHT-playerBarWidget->height())/2-50);
    countdown->setMinimumHeight(50);
    countdown->setMinimumWidth(WINDOW_WIDTH);

    messageAtTopOfScreen = new MessageAtTopOfScreen(this);
    messageAtTopOfScreen->move(0, 20);
    messageAtTopOfScreen->setMinimumWidth(WINDOW_WIDTH);

    powerGrenadeThrowing = new PowerGrenadeThrowing(this);
    powerGrenadeThrowing->move(20,10);
    powerGrenadeThrowing->setMinimumWidth(300);

    leaderboard = new Leaderboard(this);
    leaderboard->move(0,0);
    leaderboard->setMinimumWidth(WINDOW_WIDTH);
    leaderboard->setMinimumHeight(WINDOW_HEIGHT - playerBarWidget->height());

    const int gameHeight = WINDOW_HEIGHT - playerBarWidget->minimumHeight();
    pauseMenu = new PauseMenu(this);
    pauseMenu->setFixedSize(300, 180);
    pauseMenu->move((WINDOW_WIDTH - 300) / 2, (gameHeight - 180) / 2);
    connect(pauseMenu, &PauseMenu::resumeClicked, this, &MainWidget::resume);
    connect(pauseMenu, &PauseMenu::backToMenuClicked, this, &MainWidget::backToMenuRequested);

    roundOverWidget = new RoundOverWidget(this);
    roundOverWidget->setFixedSize(420, 260);
    roundOverWidget->move((WINDOW_WIDTH - 420) / 2, (gameHeight - 260) / 2);
    connect(roundOverWidget, &RoundOverWidget::playAgainClicked, this, &MainWidget::playAgainRequested);
    connect(roundOverWidget, &RoundOverWidget::backToMenuClicked, this, &MainWidget::backToMenuRequested);

    connect(mapWidget, &MapWidget::pauseRequested, this, &MainWidget::pause);
    connect(mapWidget, &MapWidget::roundOver, this, &MainWidget::showRoundOver);

    mapWidget->setPowerGrenadeThrowing(powerGrenadeThrowing);
    mapWidget->setLeaderboard(leaderboard);
    mapWidget->setMessageAtTopOfScreen(messageAtTopOfScreen);
    mapWidget->setCountdown(countdown);

    setLayout(qvboxLayout);
    qvboxLayout->addWidget(mapWidget);
    qvboxLayout->addWidget(playerBarWidget);
}

void MainWidget::pause()
{
    if (GameClock::isPaused() || mapWidget->isRoundOver())
        return;
    mapWidget->releaseControls();
    GameClock::setPaused(true);
    pauseMenu->showMenu();
}

void MainWidget::resume()
{
    if (mapWidget->isRoundOver())
        return;
    pauseMenu->setVisible(false);
    GameClock::setPaused(false);
    mapWidget->focusGame();
}

void MainWidget::showRoundOver()
{
    pauseMenu->setVisible(false);
    roundOverWidget->showResult(mapWidget->heroStats(), mapWidget->getPlayer()->getHeroStats());
}

MainWidget::~MainWidget()
{
    delete qvboxLayout;
    delete playerBarWidget;
    delete mapWidget;
}
