#-------------------------------------------------
#
# Project created by QtCreator 2014-12-22T11:55:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets quick quickwidgets serialport

TARGET = BLE_test
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    svideBle.cpp

HEADERS  += mainwindow.h \
    svideBle.h

FORMS    += mainwindow.ui

RESOURCES += \
    gui.qrc
