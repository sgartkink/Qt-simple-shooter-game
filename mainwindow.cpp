#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1024,720);
    setWindowTitle("Shooter");

    setCentralWidget(mainWidget);
    mainWidget->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mainWidget;
}
