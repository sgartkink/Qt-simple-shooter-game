# Automated tests for the game (Qt Test). Build and run out of source:
#   mkdir -p build/tests && cd build/tests && qmake ../../tests/tests.pro && make -j && make check
# The tests run offscreen unless QT_QPA_PLATFORM is set.

TARGET = tests
TEMPLATE = app

QT += testlib
CONFIG += testcase

include(../game.pri)

SOURCES += \
    main.cpp \
    test_pathfinding.cpp \
    test_weapons.cpp \
    test_fieldofview.cpp \
    test_bot.cpp \
    test_game.cpp \
    test_shotmarker.cpp \
    test_flow.cpp \
    test_sniper.cpp

HEADERS += \
    helpers.h
