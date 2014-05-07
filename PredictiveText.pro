#-------------------------------------------------
#
# Project created by QtCreator 2014-03-28T09:46:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PredictiveText
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    predictor.cpp

HEADERS  += mainwindow.h \
    predictor.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    dictionary.txt
