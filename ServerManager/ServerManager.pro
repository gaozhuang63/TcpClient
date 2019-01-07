#-------------------------------------------------
#
# Project created by QtCreator 2017-08-17T15:27:26
#
#-------------------------------------------------

QT       += core gui network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServerManager
TEMPLATE = app

CONFIG   += C++11

SOURCES += main.cpp\
        dialog.cpp \
    tcpserver.cpp \
    tcpsocket.cpp \
    threadhandle.cpp \
    new_window.cpp \
    sql.cpp \
    form.cpp

HEADERS  += dialog.h \
    tcpserver.h \
    tcpsocket.h \
    threadhandle.h \
    new_window.h \
    sql.h \
    form.h

FORMS    += dialog.ui \
    new_window.ui \
    form.ui

RESOURCES += \
    servermanager.qrc
