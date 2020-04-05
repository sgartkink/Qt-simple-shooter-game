#ifndef COUNTDOWNTOSTART_H
#define COUNTDOWNTOSTART_H

#include <QTimer>
#include <QGraphicsScene>
#include <QLabel>
#include <QObject>

class CountdownToStart : public QObject
{
    Q_OBJECT
public:
    CountdownToStart(QGraphicsScene * scene, int width, int height);
    ~CountdownToStart();

private:
    QGraphicsScene * scene;
    QTimer timer;
    int secLeft = 3;

    QLabel * lText = new QLabel("Game starts in:");
    QLabel * lSecLeft = new QLabel();

private slots:
    void countdown();
};

#endif // COUNTDOWNTOSTART_H
