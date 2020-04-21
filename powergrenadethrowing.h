#ifndef POWERGRENADETHROWING_H
#define POWERGRENADETHROWING_H

#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsOpacityEffect>

class PowerGrenadeThrowing : public QProgressBar
{
public:
    explicit PowerGrenadeThrowing(QWidget *parent = nullptr);
    ~PowerGrenadeThrowing();

    void start();
    void stop();

private:
    QPropertyAnimation * animationValue = nullptr;
    QPropertyAnimation * animationOpacity = nullptr;
};

#endif // POWERGRENADETHROWING_H
