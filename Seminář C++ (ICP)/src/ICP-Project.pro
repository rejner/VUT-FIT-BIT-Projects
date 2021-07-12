QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++1z

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    control/main.cpp \
    gui/help_window.cpp \
    gui/mainwindow.cpp \
    gui/myscene.cpp \
    gui/timetable.cpp \
    gui/intro.cpp \
    model/coordinate.cpp \
    model/stop.cpp \
    model/street.cpp \
    model/streetmap.cpp \
    model/time.cpp \
    model/transportline.cpp \
    model/vehicle.cpp

HEADERS += \
    gui/help_window.h \
    gui/mainwindow.h \
    gui/myscene.h \
    gui/timetable.h \
    gui/intro.h \
    model/coordinate.h \
    model/stop.h \
    model/street.h \
    model/streetmap.h \
    model/time.h \
    model/transportline.h \
    model/vehicle.h

FORMS += \
    gui/help_window.ui \
    gui/mainwindow.ui \
    gui/timetable.ui \
    gui/intro.ui

TRANSLATIONS += \
    ICP-Project_cs_CZ.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
