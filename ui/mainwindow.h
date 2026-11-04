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

public slots:
    void startProgram();
    void showOptions();
    void backToMenu();
    void stopProgram();

private:
    QStackedWidget * stackedWidget = new QStackedWidget(this);
    MainMenu * mainMenu = new MainMenu(this);
    MainWidget * mainWidget = new MainWidget(this);
    OptionsWidget * optionsWidget = new OptionsWidget(this);
};

#endif // MAINWINDOW_H
