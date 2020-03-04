#ifndef GRIDELEMENT_H
#define GRIDELEMENT_H

#include <QPoint>

class GridElement : public QPoint
{
public:
    GridElement(int x_, int y_);

    bool isTaken() { return taken; }
    int getParentX() { return parentX; }
    int getParentY() { return parentY; }

    void setParentX(int x) { parentX = x; }
    void setParentY(int y) { parentY = y; }
    void toggleTaken() { taken = !taken; }

private:
    bool taken = false;
    int parentX = -1;
    int parentY = -1;
};

#endif // GRIDELEMENT_H
