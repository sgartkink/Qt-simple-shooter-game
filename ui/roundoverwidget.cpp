#include "ui/roundoverwidget.h"

#include <algorithm>

RoundOverWidget::RoundOverWidget(QWidget *parent)
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
    table->setAlignment(Qt::AlignCenter);
    table->setTextFormat(Qt::PlainText);

    layout->addWidget(title);
    layout->addWidget(table);
    layout->addWidget(bPlayAgain);
    layout->addWidget(bBackToMenu);

    connect(bPlayAgain, &QPushButton::clicked, this, &RoundOverWidget::playAgainClicked);
    connect(bBackToMenu, &QPushButton::clicked, this, &RoundOverWidget::backToMenuClicked);
}

QString RoundOverWidget::resultFor(const QVector<HeroStats *> &heroes, const HeroStats *you)
{
    unsigned bestOther = 0;
    const HeroStats * bestOtherHero = nullptr;
    for (const HeroStats * hero : heroes)
        if (hero != you && (!bestOtherHero || hero->getKills() > bestOther))
        {
            bestOther = hero->getKills();
            bestOtherHero = hero;
        }

    if (!bestOtherHero || you->getKills() > bestOther)
        return "You win!";
    if (you->getKills() == bestOther)
        return "Draw";
    return QString("%1 wins").arg(bestOtherHero->getName());
}

void RoundOverWidget::showResult(const QVector<HeroStats *> &heroes, const HeroStats *you)
{
    title->setText("Time's up - " + resultFor(heroes, you));

    QVector<HeroStats *> sorted = heroes;
    std::stable_sort(sorted.begin(), sorted.end(), [](HeroStats * a, HeroStats * b)
    {
        return a->getKills() != b->getKills() ? a->getKills() > b->getKills() : a->getDeaths() < b->getDeaths();
    });
    QStringList rows;
    for (int i = 0; i < sorted.size(); i++)
        rows << QString("%1. %2 - %3 kills, %4 deaths").arg(i + 1).arg(sorted[i]->getName())
                .arg(sorted[i]->getKills()).arg(sorted[i]->getDeaths());
    table->setText(rows.join('\n'));

    setVisible(true);
    raise();
    bPlayAgain->setFocus();
}
