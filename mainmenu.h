#ifndef MAINMENU_H
#define MAINMENU_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class MainMenu : public QWidget
{
    Q_OBJECT
public:
    explicit MainMenu(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MainMenu();

private:
    QVBoxLayout *qvBoxLayout = new QVBoxLayout(this);

    QPushButton *bStartGame = new QPushButton("Start game");
    QPushButton *bOptions = new QPushButton("Options");
    QPushButton *bExit = new QPushButton("Exit");
};

#endif // MAINMENU_H
