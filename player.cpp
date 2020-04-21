#include "player.h"
#include "global_consts.h"
#include "mapwidget.h"

Player::Player(QGraphicsView *mapView, MapWidget *mapWidget)
    : Hero(mapWidget, Qt::blue, 0, 0, 10, 10), mapView(mapView)
{
    shooting = new Shooting(this, currentGun, mapWidget->getScene(), true);
    heroStats.setName("player");
}

void Player::updateLineHeroMouse(QPoint mousePoint)
{
    savedMousePoint = mousePoint;
    mappedPoint = mapView->mapToScene(mousePoint);

    lineHeroMouse.setLine(x() + size/2, y() + size/2, mappedPoint.x(), mappedPoint.y());
    setRotation(-1 * lineHeroMouse.angle());
}

void Player::nextMove()
{
    if (directionUp)
        changePosAndUpdateLine(0, -2);
    if (directionRight)
        changePosAndUpdateLine(2, 0);
    if (directionDown)
        changePosAndUpdateLine(0, 2);
    if (directionLeft)
        changePosAndUpdateLine(-2, 0);
}

void Player::changePosAndUpdateLine(int x_, int y_)
{
    if (x_ == -2 && x() >= 0)
        checkCorners(-2, 0, -2, size, -2, 0);
    else if (x_ == 2 && x() <= MAP_SIZE_X_FULL-size)
        checkCorners(size+2, 0, size+2, size, 2, 0);
    else if (y_ == -2 && y() >= 0)
        checkCorners(0, -2, size, -2, 0, -2);
    else if (y_ == 2 && y() <= MAP_SIZE_Y_FULL-size)
        checkCorners(0, size+2, size, size+2, 0, 2);
}

void Player::checkCorners(int addToX_corner1, int addToY_corner1, int addToX_corner2, int addToY_corner2, int x_, int y_)
{
    corner1 = dynamic_cast<ItemsOnScene*>(scene->itemAt(x() + addToX_corner1, y() + addToY_corner1, QTransform()));
    corner2 = dynamic_cast<ItemsOnScene*>(scene->itemAt(x() + addToX_corner2, y() + addToY_corner2, QTransform()));

    if (!corner1 && !corner2)
    {
        newMove(x_,y_);
        return;
    }
}

void Player::newMove(int x_, int y_)
{
    setPos(x() + x_, y() + y_);
    updateLineHeroMouse(savedMousePoint);

    emit (posChanged());
}

void Player::death(Bullet *b)
{
    heroStats.increaseDeath();
    resetHero();
    randNewPos();
    changeColor(Qt::white);

    if (b)
        b->getOwner()->addKill();

    QTimer::singleShot(TIME_RETURN_TO_LIFE, [this]()
    {
        this->start();
    });
    mapWidget->showCountdown("Back in:", TIME_RETURN_TO_LIFE/1000);
}
