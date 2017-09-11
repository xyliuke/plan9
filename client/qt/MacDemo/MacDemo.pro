#-------------------------------------------------
#
# Project created by QtCreator 2016-04-29T15:02:02
#
#-------------------------------------------------

QT       += core gui
QT       += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = MacDemo
TEMPLATE = app

INCLUDEPATH += ../../../common/header  \
            ../../../common/src  \
            ../../../3rd/jsoncpp/dist/  \
            ../../../3rd/boost_1_55_0/

LIBS += libcommon.a  \
    libboost_system.a \
    libboost_filesystem.a

SOURCES += main.cpp \
    webui.cpp \
    webbridge.cpp \
    biz_proxy.cpp

HEADERS  += \
    webui.h \
    webbridge.h \
    biz_proxy.h

