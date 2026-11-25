# The game itself, without main.cpp: included by shooter.pro (the game) and tests/tests.pro (the tests)

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++11
CONFIG += c++1z

# Includes are written relative to the project root, e.g. #include "characters/hero.h"
INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/core/gameclock.cpp \
    $$PWD/core/settings.cpp \
    $$PWD/ui/pausemenu.cpp \
    $$PWD/ui/roundoverwidget.cpp \
    $$PWD/ui/mainwindow.cpp \
    $$PWD/ui/mainmenu.cpp \
    $$PWD/ui/mainwidget.cpp \
    $$PWD/ui/optionswidget.cpp \
    $$PWD/ui/playerbarwidget.cpp \
    $$PWD/ui/leaderboard.cpp \
    $$PWD/ui/leaderboardslot.cpp \
    $$PWD/ui/countdown.cpp \
    $$PWD/ui/messageattopofscreen.cpp \
    $$PWD/ui/powergrenadethrowing.cpp \
    $$PWD/world/map.cpp \
    $$PWD/world/mapwidget.cpp \
    $$PWD/world/mapview.cpp \
    $$PWD/world/gridelement.cpp \
    $$PWD/world/itemsonscene.cpp \
    $$PWD/world/building.cpp \
    $$PWD/world/chest.cpp \
    $$PWD/world/shakescreen.cpp \
    $$PWD/world/fieldofview.cpp \
    $$PWD/world/shotmarker.cpp \
    $$PWD/characters/hero.cpp \
    $$PWD/characters/player.cpp \
    $$PWD/characters/bot.cpp \
    $$PWD/characters/herostats.cpp \
    $$PWD/characters/bfs_algorithm.cpp \
    $$PWD/weapons/gun.cpp \
    $$PWD/weapons/pistol.cpp \
    $$PWD/weapons/rifle.cpp \
    $$PWD/weapons/shotgun.cpp \
    $$PWD/weapons/sniper.cpp \
    $$PWD/weapons/shooting.cpp \
    $$PWD/weapons/bullet.cpp \
    $$PWD/weapons/grenade.cpp \
    $$PWD/weapons/grenadeellipseitem.cpp

HEADERS += \
    $$PWD/core/global_consts.h \
    $$PWD/core/gameclock.h \
    $$PWD/core/settings.h \
    $$PWD/ui/pausemenu.h \
    $$PWD/ui/roundoverwidget.h \
    $$PWD/ui/mainwindow.h \
    $$PWD/ui/mainmenu.h \
    $$PWD/ui/mainwidget.h \
    $$PWD/ui/optionswidget.h \
    $$PWD/ui/playerbarwidget.h \
    $$PWD/ui/leaderboard.h \
    $$PWD/ui/leaderboardslot.h \
    $$PWD/ui/countdown.h \
    $$PWD/ui/messageattopofscreen.h \
    $$PWD/ui/powergrenadethrowing.h \
    $$PWD/world/map.h \
    $$PWD/world/mapwidget.h \
    $$PWD/world/mapview.h \
    $$PWD/world/gridelement.h \
    $$PWD/world/itemsonscene.h \
    $$PWD/world/building.h \
    $$PWD/world/chest.h \
    $$PWD/world/shakescreen.h \
    $$PWD/world/fieldofview.h \
    $$PWD/world/shotmarker.h \
    $$PWD/characters/hero.h \
    $$PWD/characters/player.h \
    $$PWD/characters/bot.h \
    $$PWD/characters/herostats.h \
    $$PWD/characters/bfs_algorithm.h \
    $$PWD/weapons/gun.h \
    $$PWD/weapons/pistol.h \
    $$PWD/weapons/rifle.h \
    $$PWD/weapons/shotgun.h \
    $$PWD/weapons/sniper.h \
    $$PWD/weapons/shooting.h \
    $$PWD/weapons/bullet.h \
    $$PWD/weapons/grenade.h \
    $$PWD/weapons/grenadeellipseitem.h
