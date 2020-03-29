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

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    mapwidget.cpp \
    mapview.cpp \
    gun.cpp \
    bullet.cpp \
    hero.cpp \
    player.cpp \
    building.cpp \
    bot.cpp \
    itemsonscene.cpp \
    grenade.cpp \
    pistol.cpp \
    rifle.cpp \
    playerbarwidget.cpp \
    mainwidget.cpp \
    map.cpp \
    chest.cpp \
    grenadeellipseitem.cpp \
    shotgun.cpp \
    powergrenadethrowing.cpp \
    shooting.cpp \
    shakescreen.cpp \
    gridelement.cpp \
    bfs_algorithm.cpp \
    itemsinqpainterpath.cpp \
    mainmenu.cpp \
    optionswidget.cpp

HEADERS += \
        mainwindow.h \
    mapwidget.h \
    mapview.h \
    gun.h \
    bullet.h \
    hero.h \
    player.h \
    building.h \
    bot.h \
    itemsonscene.h \
    grenade.h \
    pistol.h \
    rifle.h \
    playerbarwidget.h \
    mainwidget.h \
    map.h \
    chest.h \
    global_consts.h \
    grenadeellipseitem.h \
    shotgun.h \
    powergrenadethrowing.h \
    shooting.h \
    shakescreen.h \
    gridelement.h \
    bfs_algorithm.h \
    itemsinqpainterpath.h \
    mainmenu.h \
    optionswidget.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
