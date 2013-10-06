#-------------------------------------------------
#
# Project created by QtCreator 2013-09-29T23:48:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = volumanq
TEMPLATE = app


SOURCES += main.cpp\
        volumanqwindow.cpp \
    aboutdialog.cpp

HEADERS  += volumanqwindow.h \
    log.h \
    aboutdialog.h

FORMS    += \
    aboutdialog.ui

RESOURCES += \
    volumanq.qrc

LIBS += -lasound
