#-------------------------------------------------
#
# Project created by QtCreator 2016-01-19T00:01:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Words
TEMPLATE = app

CONFIG	+= c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    drawing.cpp \
    structures.cpp

HEADERS  += mainwindow.h \
    drawing.h \
    structures.h
