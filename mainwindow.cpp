#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1024,720);
    setWindowTitle("Shooter");

    stackedWidget->addWidget(mainMenu);
    stackedWidget->addWidget(mainWidget);
    stackedWidget->addWidget(optionsWidget);
    stackedWidget->setCurrentWidget(mainMenu);

    setCentralWidget(stackedWidget);
    mainWidget->setMouseTracking(true);
}

void MainWindow::startProgram()
{
    stackedWidget->setCurrentWidget(mainWidget);
    mainWidget->getMapWidget()->start();
}

void MainWindow::showOptions()
{
    stackedWidget->setCurrentWidget(optionsWidget);
}

void MainWindow::backToMenu()
{
    stackedWidget->setCurrentWidget(mainMenu);
}

void MainWindow::stopProgram()
{
    qApp->quit();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainMenu;
    delete mainWidget;
    delete optionsWidget;
}
