#-------------------------------------------------
#
# Project created by QtCreator 2015-05-24T21:11:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MipsEd
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    file.cpp \
    instruction.cpp \
    memory.cpp \
    mipseditor_plugin_import.cpp \
    MipsSti.cpp \
    register.cpp

HEADERS  += mainwindow.h \
    file.h \
    headers.h \
    instruction.h \
    memory.h \
    MipsSti.h \
    register.h

FORMS    += mainwindow.ui
