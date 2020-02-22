#ifndef POWERGRENADETHROWING_H
#define POWERGRENADETHROWING_H

#include <QProgressBar>
#include <QPropertyAnimation>
#include <QTimer>

class PowerGrenadeThrowing : public QProgressBar
{
public:
    explicit PowerGrenadeThrowing(QWidget *parent = nullptr);

    void start();
    void stop();

private:
    QPropertyAnimation * animationValue = nullptr;
    QPropertyAnimation * animationOpacity = nullptr;
};

#endif // POWERGRENADETHROWING_H
