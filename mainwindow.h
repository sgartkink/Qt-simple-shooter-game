#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "mainwidget.h"
#include "mainmenu.h"
#include "optionswidget.h"

namespace Ui {
class MainWindow;
}

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
    Ui::MainWindow *ui;

    QStackedWidget * stackedWidget = new QStackedWidget(this);
    MainMenu * mainMenu = new MainMenu(this);
    MainWidget * mainWidget = new MainWidget(this);
    OptionsWidget * optionsWidget = new OptionsWidget(this);
};

#endif // MAINWINDOW_H
