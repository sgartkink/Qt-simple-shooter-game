#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>

// Shown over the game while it is paused (Esc). Esc again, or Resume, carries on.
class PauseMenu : public QWidget
{
    Q_OBJECT
public:
    explicit PauseMenu(QWidget *parent = nullptr);

    void showMenu();

signals:
    void resumeClicked();
    void backToMenuClicked();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVBoxLayout * layout = new QVBoxLayout(this);
    QLabel * title = new QLabel("Paused", this);
    QPushButton * bResume = new QPushButton("Resume", this);
    QPushButton * bBackToMenu = new QPushButton("Back to menu", this);
};

#endif // PAUSEMENU_H
