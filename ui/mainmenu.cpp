#include "ui/mainmenu.h"

MainMenu::MainMenu(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qvBoxLayout->addWidget(bContinue);
    qvBoxLayout->addWidget(bStartGame);
    bContinue->setVisible(false);
    qvBoxLayout->addWidget(bOptions);
    qvBoxLayout->addWidget(bExit);

    connect(bContinue, SIGNAL(clicked()), parent, SLOT(continueGame()));
    connect(bStartGame, SIGNAL(clicked()), parent, SLOT(startProgram()));
    connect(bOptions, SIGNAL(clicked()), parent, SLOT(showOptions()));
    connect(bExit, SIGNAL(clicked()), parent, SLOT(stopProgram()));
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
