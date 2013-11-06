#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T18:46:03
#
#-------------------------------------------------

include(libs/qextserialport/src/qextserialport.pri)

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = flashTool
TEMPLATE = app

FLASHTOOL_PATH_URI = http://....

DEFINES += FLASHTOOL_PATH_URI=\\\"$$FLASHTOOL_PATH_URI\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    progressdialog.cpp

HEADERS  += mainwindow.h \
    progressdialog.h

FORMS    += mainwindow.ui \
    progressdialog.ui

RESOURCES += \
    images.qrc
