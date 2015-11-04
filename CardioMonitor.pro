#-------------------------------------------------
#
# Project created by QtCreator 2015-03-04T20:25:01
#
#-------------------------------------------------

QT       += core gui serialport printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport bluetooth

TARGET = CardioMonitor
TEMPLATE = app


SOURCES += src/main.cpp\
           src/mainwindow.cpp \
           src/qcustomplot.cpp \
           src/cardioplot.cpp

HEADERS  += src/mainwindow.h \
            src/qcustomplot.h \
            src/cardioplot.h

FORMS    += src/ui/mainwindow.ui
