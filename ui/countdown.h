#ifndef COUNTDOWNTOSTART_H
#define COUNTDOWNTOSTART_H

#include "core/gameclock.h"
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>

class Countdown : public QWidget
{
    Q_OBJECT
public:
    Countdown(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void start(const QString &text, const int &s);

private:
    QVBoxLayout * qvBoxLayout = new QVBoxLayout(this);
    GameTimer timer;

    QLabel lText;
    QLabel lSecLeft;

    int secLeft;

private slots:
    void countdown();
};

#endif // COUNTDOWNTOSTART_H
