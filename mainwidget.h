#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "mapwidget.h"
#include "playerbarwidget.h"
#include "powergrenadethrowing.h"

class MainWidget : public QWidget
{
public:
    explicit MainWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~MainWidget();

    MapWidget * getMapWidget() { return mapWidget; }
    PowerGrenadeThrowing * getPowerGrenadeThrowing() { return powerGrenadeThrowing; }

private:
    QVBoxLayout * qvboxLayout = new QVBoxLayout(this);
    PlayerBarWidget * playerBarWidget = new PlayerBarWidget(this);
    PowerGrenadeThrowing * powerGrenadeThrowing = nullptr;
    MapWidget * mapWidget = nullptr;
};

#endif // MAINWIDGET_H
