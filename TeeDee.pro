QT += core

CONFIG += c++11

TARGET = TeeDee
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += $$PWD/src/main.cpp


INCLUDEPATH += D:\\opencv\\build\\include \
    D:\\opencv\\build\\include\\opencv \
    D:\\opencv\\build\\include\\opencv2

LIBS += -LD:\\opencv\\build\\x64\\vc14\\lib \
            -lopencv_world310d


