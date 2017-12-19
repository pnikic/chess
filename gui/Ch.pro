QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ChApp
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
QMAKE_CXXFLAGS += -I../base/
SOURCES += \
        main.cpp \
        mainwindow.cpp \
        ../base/base.cpp \
        ../base/board.cpp \
        ../base/square.cpp \
        ../base/move.cpp \
        ../base/piece.cpp \
        ../base/setsqr.cpp

HEADERS += \
        mainwindow.h \
        ../base/base.h \
        ../base/board.h \
        ../base/square.h \
        ../base/move.h \
        ../base/piece.h \
        ../base/setsqr.h

FORMS += \
        mainwindow.ui
