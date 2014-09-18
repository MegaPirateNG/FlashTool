#-------------------------------------------------
#
# Project created by QtCreator 2013-11-05T18:46:03
#
#-------------------------------------------------

QT       += core gui network xml widgets serialport

TARGET = flashTool
TEMPLATE = app
LIBS += -lz

ICON = resources/logo.icns

FLASHTOOL_PATH_URI = http://fw.megapirateng.com:8888/update.xml
FLASHTOOL_VERSION = 1.1r2

DEFINES += FLASHTOOL_PATH_URI=\\\"$$FLASHTOOL_PATH_URI\\\"
DEFINES += FLASHTOOL_VERSION=\\\"$$FLASHTOOL_VERSION\\\"

SOURCES += main.cpp\
        mainwindow.cpp \
    progressdialog.cpp \
    aboutdialog.cpp \
    F4BYFirmwareUploader.cc

HEADERS  += mainwindow.h \
    progressdialog.h \
    aboutdialog.h \
    F4BYFirmwareUploader.h

FORMS    += mainwindow.ui \
    aboutdialog.ui


RC_FILE = windows_ico.rc

TRANSLATIONS += \
    resources/translations/mpng_flashtool_german.ts \
    resources/translations/mpng_flashtool_russian.ts

RESOURCES += \
    images.qrc \
    translations.qrc
