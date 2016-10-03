QT += core

CONFIG += c++11

TARGET = Tidee
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += $$PWD/main.cpp \
    QtdProcessingTools.cpp


INCLUDEPATH += D:\\opencv\\build\\include \
    D:\\opencv\\build\\include\\opencv \
    D:\\opencv\\build\\include\\opencv2

LIBS += -LD:\\opencv\\build\\x64\\vc14\\lib \
            -lopencv_world310d

HEADERS += \
    QtdProcessingTools.h


