#ifndef ROUNDOVERWIDGET_H
#define ROUNDOVERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>

#include "characters/herostats.h"

// Shown over the frozen game when the round's time is up: who won by kills, the final table and what next.
class RoundOverWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RoundOverWidget(QWidget *parent = nullptr);

    // the result for `you` among `heroes`: most kills wins, the same top number of kills is a draw
    static QString resultFor(const QVector<HeroStats *> & heroes, const HeroStats * you);
    void showResult(const QVector<HeroStats *> & heroes, const HeroStats * you);

    QString getTitle() const { return title->text(); }
    QString getTable() const { return table->text(); }

signals:
    void playAgainClicked();
    void backToMenuClicked();

private:
    QVBoxLayout * layout = new QVBoxLayout(this);
    QLabel * title = new QLabel(this);
    QLabel * table = new QLabel(this);
    QPushButton * bPlayAgain = new QPushButton("Play again", this);
    QPushButton * bBackToMenu = new QPushButton("Back to menu", this);
};

#endif // ROUNDOVERWIDGET_H
