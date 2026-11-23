#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "ui/mainwidget.h"
#include "ui/mainmenu.h"
#include "ui/optionswidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    MainWidget * getMainWidget() { return mainWidget; }

public slots:
    void startProgram();
    void continueGame();
    void showOptions();
    void backToMenu();
    void leaveGame();     // from the game: frozen as it is, or ended if its round is over
    void stopProgram();

private:
    QStackedWidget * stackedWidget = new QStackedWidget(this);
    MainMenu * mainMenu = new MainMenu(this);
    MainWidget * mainWidget = nullptr;     // the game being played; a new one for every game

    void endGame();
    void showMenu();
    OptionsWidget * optionsWidget = new OptionsWidget(this);
};

#endif // MAINWINDOW_H
