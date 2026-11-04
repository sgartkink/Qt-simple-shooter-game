#ifndef GRIDELEMENT_H
#define GRIDELEMENT_H

#include <QPoint>
#include <QGraphicsScene>
#include <QGraphicsTextItem>

class GridElement : public QPoint
{
public:
    GridElement(int x_, int y_);

    bool isTaken() { return taken; }
    int getPosition() { return position; }

    void setPosition(int p) { position = p; }
    void toggleTaken() { taken = !taken; }

private:
    bool taken = false;
    int position = -1;
};

#endif // GRIDELEMENT_H
