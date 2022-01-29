#-------------------------------------------------
#
# Project created by QtCreator 2017-07-09T23:14:55
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ControlDeCultivos
TEMPLATE = app


SOURCES += main.cpp\
        controlcultivos.cpp \
    pagoshistoricos.cpp \
    compras.cpp

HEADERS  += controlcultivos.h \
    pagoshistoricos.h \
    compras.h

FORMS    += controlcultivos.ui \
    pagoshistoricos.ui \
    compras.ui

RESOURCES += \
    iconos.qrc
