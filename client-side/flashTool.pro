#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T18:46:03
#
#-------------------------------------------------

QT       += core gui network xml widgets serialport

TARGET = flashTool
TEMPLATE = app
LIBS += -lz

FLASHTOOL_PATH_URI = http://fw.megapirateng.com:8888/update.xml
FLASHTOOL_VERSION = 1.0

DEFINES += FLASHTOOL_PATH_URI=\\\"$$FLASHTOOL_PATH_URI\\\"
DEFINES += FLASHTOOL_VERSION=\\\"$$FLASHTOOL_VERSION\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    progressdialog.cpp \
    aboutdialog.cpp \
    arduino_stk500v2.cpp \
    arduino_intelhex.cpp

HEADERS  += mainwindow.h \
    progressdialog.h \
    aboutdialog.h \
    arduino_stk500v2.h \
    arduino_intelhex.h

FORMS    += mainwindow.ui \
    aboutdialog.ui


RC_FILE = windows_ico.rc

TRANSLATIONS += \
    resources/translations/mpng_flashtool_german.ts
    resources/translations/mpng_flashtool_russian.ts

RESOURCES += \
    images.qrc \
    translations.qrc
