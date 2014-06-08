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
    aboutdialog.cpp \
    volumeslider.cpp \
    alsamodule.cpp

HEADERS  += \
    log.h \
    aboutdialog.h \
    volumeslider.h \
    alsamodule.h

FORMS    += \
    forms/aboutdialog.ui

RESOURCES += \
    volumanq.qrc

LIBS += -lasound
