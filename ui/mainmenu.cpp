#include "ui/mainmenu.h"

MainMenu::MainMenu(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qvBoxLayout->addWidget(bContinue);
    qvBoxLayout->addWidget(bStartGame);
    bContinue->setVisible(false);
    qvBoxLayout->addWidget(bOptions);
    qvBoxLayout->addWidget(bExit);

    connect(bContinue, &QPushButton::clicked, this, &MainMenu::continueClicked);
    connect(bStartGame, &QPushButton::clicked, this, &MainMenu::startClicked);
    connect(bOptions, &QPushButton::clicked, this, &MainMenu::optionsClicked);
    connect(bExit, &QPushButton::clicked, this, &MainMenu::exitClicked);
}

void MainMenu::setGameInProgress(bool inProgress)
{
    bContinue->setVisible(inProgress);
    bStartGame->setText(inProgress ? "New game" : "Start game");
}

MainMenu::~MainMenu()
{
    delete bContinue;
    delete qvBoxLayout;
    delete bStartGame;
    delete bOptions;
    delete bExit;
}
