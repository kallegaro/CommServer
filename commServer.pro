#-------------------------------------------------
#
# Project created by QtCreator 2015-01-17T01:39:36
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = commServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    src/tcpsocketserver.cpp

HEADERS += \
    inc/tcpsocketserver.h
