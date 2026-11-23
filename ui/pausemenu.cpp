#include "ui/pausemenu.h"

PauseMenu::PauseMenu(QWidget *parent)
    : QWidget(parent)
{
    setVisible(false);
    setAutoFillBackground(true);
    QPalette p = palette();
    p.setColor(QPalette::Window, QColor(30, 30, 30));
    p.setColor(QPalette::WindowText, Qt::white);
    setPalette(p);

    QFont font = title->font();
    font.setPointSize(font.pointSize() * 2);
    font.setBold(true);
    title->setFont(font);
    title->setAlignment(Qt::AlignCenter);

    layout->addWidget(title);
    layout->addWidget(bResume);
    layout->addWidget(bBackToMenu);

    connect(bResume, &QPushButton::clicked, this, &PauseMenu::resumeClicked);
    connect(bBackToMenu, &QPushButton::clicked, this, &PauseMenu::backToMenuClicked);
}

void PauseMenu::showMenu()
{
    setVisible(true);
    raise();
    bResume->setFocus();
}

void PauseMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && !event->isAutoRepeat())
        emit resumeClicked();
    else
        QWidget::keyPressEvent(event);
}
