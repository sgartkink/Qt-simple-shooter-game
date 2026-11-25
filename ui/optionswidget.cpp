#include "ui/optionswidget.h"
#include "core/settings.h"

OptionsWidget::OptionsWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    qvBoxLayout->addWidget(&lUp);
    qvBoxLayout->addWidget(&lDown);
    qvBoxLayout->addWidget(&lRight);
    qvBoxLayout->addWidget(&lLeft);
    qvBoxLayout->addWidget(&lReload);
    qvBoxLayout->addWidget(&lGrenade);
    qvBoxLayout->addWidget(&lChest);
    qvBoxLayout->addWidget(&lGuns);
    qvBoxLayout->addWidget(&lScope);
    qvBoxLayout->addWidget(&lLeaderboard);
    qvBoxLayout->addWidget(&lPause);

    for (int minutes : Settings::ROUND_MINUTES_CHOICES)
        roundLength->addItem(QString("%1 minute%2").arg(minutes).arg(minutes == 1 ? "" : "s"), minutes);
    roundLength->setCurrentIndex(Settings::ROUND_MINUTES_CHOICES.indexOf(Settings::roundMinutes()));
    connect(roundLength, QOverload<int>::of(&QComboBox::currentIndexChanged), roundLength, [this](int index)
    {
        Settings::setRoundMinutes(roundLength->itemData(index).toInt());
    });
    roundLayout->addWidget(&lRoundLength);
    roundLayout->addWidget(roundLength);
    roundLayout->addWidget(&lRoundLocked);
    lRoundLocked.setVisible(false);
    roundLayout->addStretch();
    qvBoxLayout->addLayout(roundLayout);

    qvBoxLayout->addWidget(bBack);

    connect(bBack, &QPushButton::clicked, this, &OptionsWidget::backClicked);
}

void OptionsWidget::setRoundLengthLocked(bool locked)
{
    roundLength->setEnabled(!locked);
    lRoundLocked.setVisible(locked);
}
