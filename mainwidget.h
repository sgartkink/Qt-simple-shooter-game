#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

#include "mapwidget.h"
#include "playerbarwidget.h"

class MainWidget : public QWidget
{
public:
    explicit MainWidget(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

private:
    QVBoxLayout * qvboxLayout = new QVBoxLayout(this);
    PlayerBarWidget * playerBarWidget = new PlayerBarWidget(this);
    MapWidget * mapWidget = new MapWidget(playerBarWidget, this);
};

#endif // MAINWIDGET_H
