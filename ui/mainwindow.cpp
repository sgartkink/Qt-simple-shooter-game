#include "ui/mainwindow.h"
#include "core/global_consts.h"
#include "core/gameclock.h"
#include "core/settings.h"

#include <QApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("Shooter");

    stackedWidget->addWidget(mainMenu);
    stackedWidget->addWidget(optionsWidget);
    stackedWidget->setCurrentWidget(mainMenu);

    setCentralWidget(stackedWidget);
}

void MainWindow::startProgram()
{
    endGame();
    mainWidget = new MainWidget(this);
    mainWidget->setMouseTracking(true);
    // queued: both are clicked inside the game, which goes away with them
    connect(mainWidget, &MainWidget::backToMenuRequested, this, &MainWindow::leaveGame, Qt::QueuedConnection);
    connect(mainWidget, &MainWidget::playAgainRequested, this, &MainWindow::startProgram, Qt::QueuedConnection);
    stackedWidget->addWidget(mainWidget);
    stackedWidget->setCurrentWidget(mainWidget);

    MapWidget * mapWidget = mainWidget->getMapWidget();
    mapWidget->setRoundLength(Settings::roundMinutes() * 60000);
    mapWidget->start();
    mapWidget->focusGame();
}

void MainWindow::endGame()
{
    // the game and everything in it is freed; the clock runs again for the next one
    if (mainWidget)
    {
        stackedWidget->removeWidget(mainWidget);
        delete mainWidget;
        mainWidget = nullptr;
    }
    GameClock::reset();
    mainMenu->setGameInProgress(false);
}

void MainWindow::continueGame()
{
    if (!mainWidget)
        return;
    stackedWidget->setCurrentWidget(mainWidget);
    mainWidget->resume();
}

void MainWindow::leaveGame()
{
    // left from the pause menu the game is already paused and stays so behind the menu; a finished round can't go on
    if (mainWidget && mainWidget->getMapWidget()->isRoundOver())
        endGame();
    else if (mainWidget)
        mainWidget->pause();
    showMenu();
}

void MainWindow::showOptions()
{
    optionsWidget->setRoundLengthLocked(mainWidget != nullptr);
    stackedWidget->setCurrentWidget(optionsWidget);
}

void MainWindow::backToMenu()
{
    showMenu();
}

void MainWindow::showMenu()
{
    mainMenu->setGameInProgress(mainWidget != nullptr);
    stackedWidget->setCurrentWidget(mainMenu);
}

void MainWindow::stopProgram()
{
    qApp->quit();
}

MainWindow::~MainWindow()
{
    endGame();          // first: it updates the menu
    delete mainMenu;
    delete optionsWidget;
}
