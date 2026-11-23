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
    QLabel lGuns {"1 - PISTOL, 2 - RIFLE, 3 - SHOTGUN"};
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

private:

    QPushButton * bBack = new QPushButton("Back");
};

#endif // OPTIONSWIDGET_H
