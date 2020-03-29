#include "mainmenu.h"

MainMenu::MainMenu(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qvBoxLayout->addWidget(bStartGame);
    qvBoxLayout->addWidget(bOptions);
    qvBoxLayout->addWidget(bExit);

    connect(bStartGame, SIGNAL(clicked()), parent, SLOT(startProgram()));
    connect(bOptions, SIGNAL(clicked()), parent, SLOT(showOptions()));
    connect(bExit, SIGNAL(clicked()), parent, SLOT(stopProgram()));
}

MainMenu::~MainMenu()
{
    delete qvBoxLayout;
    delete bStartGame;
    delete bOptions;
    delete bExit;
}
