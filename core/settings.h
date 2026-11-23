#ifndef SETTINGS_H
#define SETTINGS_H

#include <QVector>

// Player-chosen options, remembered between runs (an ini file in the user's config folder).
namespace Settings
{
    const QVector<int> ROUND_MINUTES_CHOICES {1, 3, 5};
    const int DEFAULT_ROUND_MINUTES = 3;

    int roundMinutes();                     // one of ROUND_MINUTES_CHOICES
    void setRoundMinutes(int minutes);      // ignored unless it is one of them
}

#endif // SETTINGS_H
