#-------------------------------------------------
#
# Project created by QtCreator 2018-08-20T09:12:54
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = start
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

# library for theme
unix:!macx: LIBS += -lcprime

FORMS += \
    sessionsavedialog.ui \
    start.ui

HEADERS += \
    sessionsavedialog.h \
    slidingstackedwidget.h \
    start.h

SOURCES += \
    main.cpp \
    sessionsavedialog.cpp \
    slidingstackedwidget.cpp \
    start.cpp

RESOURCES += \
    icons.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


