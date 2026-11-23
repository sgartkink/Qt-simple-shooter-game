#include "core/settings.h"

#include <QSettings>

namespace
{
    const char * ORGANIZATION = "simple-shooter";
    const char * APPLICATION = "shooter";
    const char * ROUND_MINUTES_KEY = "round/minutes";
}

int Settings::roundMinutes()
{
    QSettings store(QSettings::IniFormat, QSettings::UserScope, ORGANIZATION, APPLICATION);
    const int minutes = store.value(ROUND_MINUTES_KEY, DEFAULT_ROUND_MINUTES).toInt();
    return ROUND_MINUTES_CHOICES.contains(minutes) ? minutes : DEFAULT_ROUND_MINUTES;
}

void Settings::setRoundMinutes(int minutes)
{
    if (!ROUND_MINUTES_CHOICES.contains(minutes))
        return;
    QSettings store(QSettings::IniFormat, QSettings::UserScope, ORGANIZATION, APPLICATION);
    store.setValue(ROUND_MINUTES_KEY, minutes);
}
