#-------------------------------------------------
#
# Project created by QtCreator 2015-09-09T09:22:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChainOfPerls
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        game.cpp \
        autoplayer.cpp \
    autoplayerthread.cpp

HEADERS  += mainwindow.h \
         game.h \
         autoplayer.h \
    autoplayerthread.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

RC_FILE += \
    ChainOfPerls.rc
