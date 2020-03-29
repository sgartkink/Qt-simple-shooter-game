#ifndef OPTIONSWIDGET_H
#define OPTIONSWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class OptionsWidget : public QWidget
{
public:
    explicit OptionsWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~OptionsWidget();

private:
    QVBoxLayout *qvBoxLayout = new QVBoxLayout(this);
    QLabel * lUp = new QLabel("Up: W");
    QLabel * lDown = new QLabel("Down: S");
    QLabel * lRight = new QLabel("Right: D");
    QLabel * lLeft = new QLabel("Left: A");
    QLabel * lReload = new QLabel("Reload: R");
    QLabel * lGrenade = new QLabel("Start throwing grenade: Q (hold to throw stronger)");
    QLabel * lChest = new QLabel("Open chest: E");
    QLabel * lGuns = new QLabel("1 - PISTOL, 2 - RIFLE, 3 - SHOTGUN");

    QPushButton * bBack = new QPushButton("Back");
};

#endif // OPTIONSWIDGET_H
