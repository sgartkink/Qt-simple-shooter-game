#-------------------------------------------------
#
# Project created by QtCreator 2019-11-15T21:58:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = shooter
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
CONFIG += c++1z

# Includes are written relative to the project root, e.g. #include "characters/hero.h"
INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp \
    ui/mainwindow.cpp \
    ui/mainmenu.cpp \
    ui/mainwidget.cpp \
    ui/optionswidget.cpp \
    ui/playerbarwidget.cpp \
    ui/leaderboard.cpp \
    ui/leaderboardslot.cpp \
    ui/countdown.cpp \
    ui/messageattopofscreen.cpp \
    ui/powergrenadethrowing.cpp \
    world/map.cpp \
    world/mapwidget.cpp \
    world/mapview.cpp \
    world/gridelement.cpp \
    world/itemsonscene.cpp \
    world/building.cpp \
    world/chest.cpp \
    world/shakescreen.cpp \
    world/fieldofview.cpp \
    characters/hero.cpp \
    characters/player.cpp \
    characters/bot.cpp \
    characters/herostats.cpp \
    characters/bfs_algorithm.cpp \
    weapons/gun.cpp \
    weapons/pistol.cpp \
    weapons/rifle.cpp \
    weapons/shotgun.cpp \
    weapons/shooting.cpp \
    weapons/bullet.cpp \
    weapons/grenade.cpp \
    weapons/grenadeellipseitem.cpp

HEADERS += \
    core/global_consts.h \
    ui/mainwindow.h \
    ui/mainmenu.h \
    ui/mainwidget.h \
    ui/optionswidget.h \
    ui/playerbarwidget.h \
    ui/leaderboard.h \
    ui/leaderboardslot.h \
    ui/countdown.h \
    ui/messageattopofscreen.h \
    ui/powergrenadethrowing.h \
    world/map.h \
    world/mapwidget.h \
    world/mapview.h \
    world/gridelement.h \
    world/itemsonscene.h \
    world/building.h \
    world/chest.h \
    world/shakescreen.h \
    world/fieldofview.h \
    characters/hero.h \
    characters/player.h \
    characters/bot.h \
    characters/herostats.h \
    characters/bfs_algorithm.h \
    weapons/gun.h \
    weapons/pistol.h \
    weapons/rifle.h \
    weapons/shotgun.h \
    weapons/shooting.h \
    weapons/bullet.h \
    weapons/grenade.h \
    weapons/grenadeellipseitem.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
