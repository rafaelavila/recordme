#-------------------------------------------------
#
# Project created by QtCreator 2015-08-29T00:21:42
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FilmaEu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    read_write.cpp

HEADERS  += mainwindow.h \
    read_write.h

LIBS += `pkg-config opencv --libs`

FORMS    += mainwindow.ui
