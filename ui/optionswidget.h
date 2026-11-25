#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QHBoxLayout>

class OptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OptionsWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

private:
    QVBoxLayout *qvBoxLayout = new QVBoxLayout(this);
    QLabel lUp {"Up: W"};
    QLabel lDown {"Down: S"};
    QLabel lRight {"Right: D"};
    QLabel lLeft {"Left: A"};
    QLabel lReload {"Reload: R"};
    QLabel lGrenade {"Start throwing grenade: Q (hold to throw stronger)"};
    QLabel lChest {"Open chest: E"};
    QLabel lGuns {"1 - PISTOL, 2 - RIFLE, 3 - SHOTGUN, 4 - SNIPER RIFLE"};
    QLabel lScope {"Sniper rifle: right click to look through the scope, hold Shift to move the view with the mouse"};
    QLabel lLeaderboard {"Show leaderboard: X"};
    QLabel lPause {"Pause: Esc"};

    QHBoxLayout * roundLayout = new QHBoxLayout();
    QLabel lRoundLength {"Round length:"};
    QComboBox * roundLength = new QComboBox();
    QLabel lRoundLocked {"(a game is in progress: start a new game to change it)"};

public:
    QComboBox * getRoundLengthBox() { return roundLength; }
    // a game is frozen behind the menu: its round length can't change under it
    void setRoundLengthLocked(bool locked);

signals:
    void backClicked();

private:

    QPushButton * bBack = new QPushButton("Back");
};

#endif // OPTIONSWIDGET_H
