#-------------------------------------------------
#
# Project created by QtCreator 2019-11-15T21:58:51
#
#-------------------------------------------------

TARGET = shooter
TEMPLATE = app

include(game.pri)

SOURCES += \
    main.cpp

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
