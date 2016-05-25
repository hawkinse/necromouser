#-------------------------------------------------
#
# Project created by QtCreator 2015-07-31T20:04:16
#
#-------------------------------------------------

INCLUDEPATH += "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include"
DEPENDPATH += "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Bin"


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Necromouser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serialmousereader.cpp \
    mousesysreader.cpp \
    microsoftreader.cpp \
    packetparser.cpp

HEADERS  += mainwindow.h \
    serialmousereader.h \
    mousesysreader.h \
    microsoftreader.h \
    defines.h \
    packetparser.h

FORMS    += mainwindow.ui

#WINVER to enable SendInput in win32 API
#USING_V110_SDK71 to work around macro redefinition issue from qextserialport
DEFINES += "WINVER=0x0500" + "_USING_V110_SDK71_"

LIBS += -L"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\AdvAPI32.Lib" -L"C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib\User32.Lib"

include (3rdparty/qextserialport/src/qextserialport.pri)
