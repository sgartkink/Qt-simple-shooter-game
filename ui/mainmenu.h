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

    // a game is frozen behind the menu: offer to continue it, and starting means a new one
    void setGameInProgress(bool inProgress);

signals:
    void continueClicked();
    void startClicked();
    void optionsClicked();
    void exitClicked();

private:
    QVBoxLayout *qvBoxLayout = new QVBoxLayout(this);

    QPushButton *bContinue = new QPushButton("Continue");
    QPushButton *bStartGame = new QPushButton("Start game");
    QPushButton *bOptions = new QPushButton("Options");
    QPushButton *bExit = new QPushButton("Exit");
};

#endif // MAINMENU_H
