#ifndef MESSAGEATTOPOFSCREEN_H
#define MESSAGEATTOPOFSCREEN_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QTimer>

class MessageAtTopOfScreen : public QWidget
{
public:
    MessageAtTopOfScreen(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    void showMessage(QString text);

private:
    QGridLayout * gridLayout = new QGridLayout(this);
    QLabel label;
    QPropertyAnimation * animation = nullptr;
};

#endif // MESSAGEATTOPOFSCREEN_H
