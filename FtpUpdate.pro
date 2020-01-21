#-------------------------------------------------
#
# Project created by QtCreator 2019-12-22T22:16:53
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FtpUpdate
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



INCLUDEPATH += ./include
DEPENDPATH += ./include
QMAKE_LIBDIR += ./include

QMAKE_LIBDIR += ./lib


ROOT = "../libs/boost_1_67_vs2017"


INCLUDEPATH += $${ROOT}/include/boost-1_67
QMAKE_INCDIR += $${ROOT}/include/boost-1_67
QMAKE_LIBDIR += $${ROOT}/lib

LIBS += libboost_system-vc141-mt-x64-1_67.lib
LIBS += libboost_thread-vc141-mt-x64-1_67.lib

RC_FILE = app.rc

SOURCES += main.cpp\
    update.cpp \
    cftp_c.cpp \
    http.cpp \
    version.cpp \
    nsysconfig.cpp \
    checkversion.cpp \
    ftpconfigsetting.cpp \
    dialogloadding.cpp

HEADERS  += \
    update.h \
    cftp_c.h \
    global.h \
    http.h \
    version.h \
    nsysconfig.h \
    checkversion.h \
    ftpconfigsetting.h \
    dialogloadding.h

FORMS    += \
    update.ui \
    checkversion.ui \
    ftpconfigsetting.ui \
    dialogloadding.ui
